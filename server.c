/**
 * A simple HTTP Server, returns the inRequested files or a 404 Error
 * Based on the code provided in Lab-5
 *
 * Author:	         Luke Hedt - 832153
 * Email/Login ID:   lhedt@student.unimelb.edu.au
 * Date:	         2018/04/05
 * Name:	         server.c
 * Purpose:	Responds to HTTP inRequests with valid responses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include "respond.h"

#define BUFFSIZE 4096
#define SHORTBUFF 256
#define OOFFSET 1
#define DEF_WEB "./TestScript/test"

/* ************************************************************************* */

/**
 * Ensures all data is sent through the TCP Socket
 * Based on code from Beej's Guide to Netowrk Programming
 * http://beej.us/guide/bgnet/html/single/bgnet.html
 */

int sendall(int sockfd, char *buf, int *len) {
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
 * Main Loop
 * Based on code from Lab 4 in COMP30023
 *
 */
int main(int argc, char **argv)
{
	int sockfd, newsockfd, portno;// clilen;
	char buffer[BUFFSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	int reqLen;
    char* reply;
    char* fileReq;
    char* webRoot = (char*)malloc(BUFFSIZE*sizeof(char));
    int replyLen;
    struct sockaddr_in *sin;

    if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);

	/* Write default file root if it's not specified */
	} else if(argc == 2) {
		webRoot = DEF_WEB;

	} else {
		webRoot = argv[2];
	}

	 /* Create TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

    /* Sets socket to allow port reuse when server stops quickly */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1},
                sizeof(int)) < 0) {
            perror("setsockopt(SO_REUSEADDR) failed");
    }

	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for
	 this machine */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    /* store in machine-neutral format */
	serv_addr.sin_port = htons(portno);

	 /* Bind address to the socket */
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(EXIT_FAILURE);
	}

	/* Listen on socket - means we're ready to accept connections -
	 incoming connection requests will be queued */
	listen(sockfd,5);
    printf("Server: Listening on port %d\n", portno);
	clilen = sizeof(cli_addr);

	/* Accept a connection - block until a connection is ready to
	 be accepted. Get back a new file descriptor to communicate on. */
	newsockfd = accept(	sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
		perror("ERROR on accept");
		exit(EXIT_FAILURE);
	}

    /* Report Arriving Connection */
    sin = (struct sockaddr_in*)&cli_addr;
    unsigned char *ip = (unsigned char*)&sin->sin_addr.s_addr;
    printf("Got Connection from %d.%d.%d.%d:%d\n", ip[0],ip[1],ip[2],ip[3],
            sin->sin_port);

    /* Zero buffer to read into */
	bzero(buffer, BUFFSIZE);

	/* Read characters from the connection, then process */
	reqLen = read(newsockfd, buffer, BUFFSIZE - OOFFSET);

	if (reqLen < 0) {
		perror("ERROR reading from socket");
		exit(EXIT_FAILURE);
	}

	printf("Here is the Incoming Request: \n\n%s\n", buffer);

    fileReq = parseRequest(buffer);

    /* Handle inRequest */
    reply = (char*)malloc(BUFFSIZE*sizeof(char));
    assert(reply);
    reply = respond(webRoot, fileReq);
    replyLen = strlen(reply);

    printf("Sending the message:\n%s\n\n", reply);

    /* Check that everything sends without error */
    if (sendall(newsockfd,reply, &replyLen) == -1) {
        perror("Error sending file");
    }

	/* close socket */
    printf("Closing connection to %d.%d.%d.%d:%d\n", ip[0],ip[1],ip[2],ip[3],
                    sin->sin_port);
	close(sockfd);
    close(newsockfd);

	return 0;
}


/* ************************************************************************* */
