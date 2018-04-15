/**
 * Author:			Luke Hedt - 832153
 * Email/Login ID:	lhedt@student.unimelb.edu.au
 * DateBegan:		2018/04/05
 * Name:			respond.c
 * Purpose: 		Contains all the code and logic for the HTTP handler.
 */

#define BUFFSIZE 4096
#define SHORTBUFF 256
#define STARTFILE 4
#define FOF_TRUE 1
#define FOF_FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include "respond.h"

/* ************************************************************************* */

/*
 * Concats 2 strings
 * Based on code from
 * https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c
 */
char* concat(char *s1, char *s2)
{
	/* +1 for the null-terminator */
	char *result = malloc(strlen(s1)+strlen(s2)+1);
	assert(result);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

/* ************************************************************************* */

/*
 * Reads the requested file.
 * Based on code from
 * https://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
 */
char* readFile(char* filename, long int* fileLen, char* webRoot) {

	ssize_t numRead;

	/* Make sure we're not reading a folder file */
	if((filename[strlen(filename)-1] == '/') || (strcmp(filename, webRoot) == 0)) {
		return NULL;
	}
	/* Open the file, make sure it's there and find the length*/
	FILE* f = fopen(filename, "rt");
	/* If the file isn't there, return a 404 */
	if(f == NULL) {
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long length = ftell(f);
	fseek(f, 0, SEEK_SET);

	/* Make the buffer big enough to fit the entire file */
	char* buffer = (char*)malloc(length + 1);
	assert(buffer);

	/* Add an end-of-string char, then read the file */
	buffer[length] = '\0';
	numRead = fread(buffer, sizeof(char), length, f);

	*fileLen = (long int)numRead;

	/* Close the file and gimme my data */
	fclose(f);

	return buffer;
}

/* ************************************************************************* */


/*
 * Gets the current time for the Time Header
 * Code Based on pmg's response:
 * https://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c
 */
char* getCurrTime() {
	char* curtime = (char*)malloc(SHORTBUFF);
	assert(curtime);
	time_t now = time(0);
	struct tm tm = *gmtime(&now);

	strftime(curtime, SHORTBUFF, "%a, %d %b %Y %H:%M:%S %Z\n", &tm);


	return curtime;
}


/* ************************************************************************* */

/*
 * Extremely primitive MIME Handler.
 * Returns the MIME type of the requested file.
 */
char* getMimeType(char* request) {

	/* Get the last '.' in the filename */
	char* ext = strrchr(request, '.');
	if (!ext) {
		return NULL;
	}

	/* Work out it's type and return the MIME type */
	if(strcmp(ext, ".html") == 0) {
		return "text/html\n";

	} else if(strcmp(ext, ".css") == 0) {
		return "text/css\n";

	} else if((strcmp(ext, ".jpg") == 0) || (strcmp(ext, ".jpeg") == 0)) {
		return "image/jpeg\n";

	} else if(strcmp(ext, ".js") ==  0) {
		return "application/javascript\n";
	}

	return NULL;
}

/* ************************************************************************* */

/*
 * Generates the required HTTP Headers.
 */
char* genHeaders(char* content, long int *contentLen, char* requestPath, int FoF) {
	char *mainhdr, *lenhdr, *conhdr, *typehdr, *timehdr;
	/* This way of defining response iterations is tiring but if I don't it leaks
	memory big time */
	char *response1, *response2, *response3;
	char *response4, *response5;
	char *curtime, *mimeType;

	/* Treat 404 Errors slightly differently due to them being Hardcoded */
	if(FoF == FOF_TRUE) {
		mainhdr = "HTTP/1.0 404 Not Found\n";
		typehdr = "Content-Type: text/html\n";

	} else {
		mainhdr = "HTTP/1.0 200 OK\n";
		typehdr = (char*)malloc(SHORTBUFF);
		assert(typehdr);
		mimeType = getMimeType(requestPath);
		sprintf(typehdr, "Content-Type: %s\n", mimeType);
	}

	/* Standard Headers */
	lenhdr = (char*)malloc(SHORTBUFF);
	assert(lenhdr);
	sprintf(lenhdr, "Content-Length: %ld\n", *contentLen);
	conhdr = "Connection: Closed\n";
	curtime = getCurrTime();
	timehdr = concat("Date: ", curtime);
	free(curtime);

	/* Join all of the individual headers and free the corresponding memory
	blocks */
	response1 = concat(mainhdr, timehdr);
	free(timehdr);
	response2 = concat(response1, lenhdr);
	free(response1);
	free(lenhdr);
	response3 = concat(response2, conhdr);
	free(response2);
	response4 = concat(response3, typehdr);
	free(response3);
	free(typehdr);
	response5 = concat(response4, "\n");
	free(response4);

	return response5;

	}


/* ************************************************************************* */

/*
 * Generates a 404 message and returns it.
 * Page is hardcoded for sake of simplicity & sake of remote automated testing
 *  No other good reason not to have a nicer 404 page which is loaded.
 */
char* fourohfour() {
	char* response;
	/* 404 Page Content */
	response = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
	"<html><head>\n"
	"<title>404: File Not Found</title>\n"
	"<style>\n"
	".center {\n"
		"max-width: 600px;\n"
		"margin: auto;\n"
	"}\n"
	"</style></head>\n"
	"<body>\n"
		"<div class=\"center\">\n"
		"<h1>404: File Not Found</h1>\n"
		"<p>We couldn't find the file you requested."
		"Perhaps there's a typo in your URL?</p>\n"
		"</div>\n"
	"</body></html>\n";


	response = genHeaders(response, (long int*)strlen(response), NULL, FOF_TRUE);

	return response;
}


/* ************************************************************************* */

/*
 * Forms the response to the HTTP Request
 */
char* respond(char* webRoot, char* request, char** r_content, long int *contentLen) {
	char* content;
	char* response;
	char* httpReq = concat(webRoot, request);

	/* Gets the file */
	printf("Getting File %s\n", httpReq);
	content = readFile(httpReq, contentLen, webRoot);

	if(content == NULL) {
		printf("File \"%s\" does not exist or is not valid. Sending 404.\n\n",
				httpReq);
		free(httpReq);
		return fourohfour();
	}

	response = genHeaders(content, contentLen, request, FOF_FALSE);
	r_content = &content;

	free(httpReq);
	free(content);

	return response;
}

/* ************************************************************************* */

/*
 * Parse's the HTTP request and finds the requested file string.
 */
char* parseRequest(char* httpReq) {
	char* fileReq;
	int readChar = STARTFILE;
	int writeChar = 0;

	/* Open a request buffer */
	fileReq = (char*)malloc(SHORTBUFF);
	assert(fileReq);

	/* Get the requested file & path */
	while((httpReq[readChar] != ' ') && (writeChar < SHORTBUFF)) {
		fileReq[writeChar] = httpReq[readChar];
		writeChar++;
		readChar++;
	}
	/* Add the String Terminator */
	fileReq[writeChar] = '\0';

	return fileReq;
}

/* ************************************************************************* */

/**
 * Ensures all data is sent through the TCP Socket
 * Based on code from Beej's Guide to Netowrk Programming
 * http://beej.us/guide/bgnet/html/single/bgnet.html
 */

int sendall(int sockfd, char *buf, long int *len) {
	// how many bytes we've sent
	int total = 0;
	// how many we have left to send
	int bytesleft = *len;
	int sentLen;

	while(total < *len) {
		sentLen = send(sockfd, buf+total, bytesleft, 0);
		if (sentLen == -1) { break; }
		total += sentLen;
		bytesleft -= sentLen;
	}

	// return number actually sent here
	*len = total;

	// return -1 on failure, 0 on success
	return sentLen==-1?-1:0;
}

/* ************************************************************************* */

/*
 * Handles connection as a separate thread.
 * REQUIRES: Threadargs Structure
 */
void* conn_handler(void* thread_args) {
	int reqLen;
	long int r_hders_len;
	char buffer[BUFFSIZE];
	char* r_hders;
	char* fileReq;
	char* content;
	long int contentLen;
	struct sockaddr_in *sin;


	ThreadArgs* ta = (ThreadArgs*)thread_args;
	int clientsockfd = ta->clientsockfd;
	struct sockaddr_in *cli_addr = ta->cli_addr;
	char* webRoot = ta->webRoot;

	/* Report Arriving Connection */
	sin = (struct sockaddr_in*)&cli_addr;
	unsigned char *ip = (unsigned char*)&sin->sin_addr.s_addr;
	printf("Got Connection from %d.%d.%d.%d:%d\n", ip[0],ip[1],ip[2],ip[3],
		 sin->sin_port);

	/* Zero buffer to read into */
	bzero(buffer, BUFFSIZE);

	/* Read characters from the connection, then process */
	reqLen = read(clientsockfd, buffer, BUFFSIZE - 1);

	if (reqLen < 0) {
		perror("ERROR reading from socket");
		exit(EXIT_FAILURE);
	}

	printf("Here is the Incoming Request: \n\n%s\n", buffer);

	fileReq = parseRequest(buffer);

	/* Handle inRequest */
	r_hders = respond(webRoot, fileReq, &content, &contentLen);
	r_hders_len = strlen(r_hders);

	/* Print response */
	printf("Sending the message:\n%s", r_hders);
	int i = 0;
	while(i<contentLen) {
		printf("%c", content[i++]);
	}
	printf("\n");

	/* Check that everything sends without error */
	if (sendall(clientsockfd, r_hders, &r_hders_len) == -1) {
		perror("Error sending Headers");
	}
	if (sendall(clientsockfd, content, &contentLen) == -1) {
		perror("Error sending content");
	}

	free(r_hders);
	free(fileReq);

	/* close socket */
	printf("Closing connection to %d.%d.%d.%d:%d\n", ip[0],ip[1],ip[2],ip[3],
				sin->sin_port);

	close(clientsockfd);

	return 0;

}

/* ************************************************************************* */
