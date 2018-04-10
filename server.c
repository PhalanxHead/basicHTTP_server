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
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include "respond.h"

#define BUFFSIZE 256

/*****************************************************************************/

int main(int argc, char **argv) {
	int sockfd, newsockfd, portno;// clilen;
	char buffer[BUFFSIZE];
	char* reply;
	char* webRoot;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	int n, offset;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);

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
	serv_addr.sin_port = htons(portno);  // store in machine-neutral format

	 /* Bind address to the socket */

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	/* Listen on socket - means we're ready to accept connections -
	 incoming connection requests will be queued */

	listen(sockfd,5);

	clilen = sizeof(cli_addr);

	/* Accept a connection - block until a connection is ready to
	 be accepted. Get back a new file descriptor to communicate on. */

	newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);

	if (newsockfd < 0) {
		perror("ERROR on accept");
		exit(1);
	}

	bzero(buffer, BUFFSIZE);

	/* Read characters from the connection,
		then process */

	n = read(newsockfd,buffer, BUFFSIZE-1);

	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	/* Code based on Beej's Guide to Network Programming
	 *
     * http://beej.us/guide/bgnet/html/single/bgnet.html
	 */
	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1) {
				perror("send");
			}
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	char* request = "";
    reply = respond(webRoot, request);
	n = send(newsockfd, reply, strlen(reply), 0);

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}

	/* close socket */

	close(sockfd);

	return 0;
}


/**
 * Ensures all data is sent through the TCP Socket
 * Based on code from Beej's Guide to Netowrk Programming
 * http://beej.us/guide/bgnet/html/single/bgnet.html
 */

int sendall(int s, char *buf, int *len) {
	// how many bytes we've sent
    int total = 0;
	// how many we have left to send
    int bytesleft = *len;
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

	// return number actually sent here
    *len = total;

	// return -1 on failure, 0 on success
    return n==-1?-1:0;
}
