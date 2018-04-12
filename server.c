/**
 * A simple HTTP Server, returns the requested files or a 404 Error
 * Based on the code provided in Lab-5
 *
 * Author:	Luke Hedt - 832153
 * Date:	2018/04/05
 * Name:	server.c
 * Purpose:	Responds to HTTP Requests with valid responses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "respond.h"

#define BUFFSIZE 4096

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
    int n;

    while(total < *len) {
        n = send(sockfd, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

	// return number actually sent here
    *len = total;

	// return -1 on failure, 0 on success
    return n==-1?-1:0;
}

/* ************************************************************************* */

/**
 * Main Accept Loop, allowing forking and HTTP response.
 *
 * Code based on Beej's Guide to Network Programming
 * http://beej.us/guide/bgnet/html/single/bgnet.html
 */
int server_loop(int sockfd, char* webRoot) {
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	int newfd;
	int replyLen;
	char* reply;
	char* request = "";

	/* Accept connections ad infinitum */
	while(1) {
		sin_size = sizeof(their_addr);
		newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (newfd == -1) {
			perror("accept");
			continue;
		}

		printf("Got Connection.\n");

		/* Start a child process to handle the request */
		if (!fork()) {
			/* The child isn't listening for new conenctions so we close
			the listener socket ubder the child */
			close(sockfd);

			/* Handle Request */
			reply = respond(webRoot, request);
			replyLen = strlen(reply);

			/* Check that everything sends without error */
			if (sendall(newfd,reply, &replyLen) == -1) {
				perror("send");
			}
			/* Close the socket and kill the child */
			close(newfd);
			exit(0);
		}
		/* Close the child socket under the parent */
		close(newfd);
	}

	return 0;
}

/* ************************************************************************* */

int main(int argc, char **argv) {
	int sockfd, newsockfd, portno;// clilen;
	char buffer[BUFFSIZE];
	char* webRoot;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	int n, offset;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);

	/* Write default file root if it's not specified */
	} else if(argc == 2) {
		webRoot = "./";

	} else {
		webRoot = argv[2];
	}

	 /* Create TCP socket */

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}


	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for
	 this machine */

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// store in machine-neutral format
	serv_addr.sin_port = htons(portno);

	 /* Bind address to the socket */

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	/* Listen on socket - means we're ready to accept connections -
	 incoming connection requests will be queued */

	listen(sockfd,5);

	printf("server: waiting for connections...\n");

	server_loop(sockfd, webRoot);

	/* close socket */

	close(sockfd);

	return 0;
}

/* ************************************************************************* */
