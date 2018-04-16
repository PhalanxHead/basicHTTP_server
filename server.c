/**
 * A simple HTTP Server, returns the inRequested files or a 404 Error
 * Based on the code provided in Lab-5
 *
 * Author:			Luke Hedt - 832153
 * Email/Login ID:	lhedt@student.unimelb.edu.au
 * Date:			2018/04/05
 * Name:			server.c
 * Purpose:			Responds to HTTP inRequests with valid responses.
 *
 * Compliation: Compile with make
 */

#include <stdio.h>
#include <stdlib.h>
#include "respond.h"

#define BUFFSIZE 4096
#define SHORTBUFF 256
#define DEF_WEB "./TestScript/test"


/* ************************************************************************** */

/*
 * Main Loop
 * Based on code from Lab 4 in COMP30023
 */
int main(int argc, char **argv)
{
	int sockfd, clientsockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	char* webRoot;
	pthread_t thread_id;
	socklen_t clilen;

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

	/* Init the server address and set the port number */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for this machine */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	/* store in machine-neutral format */
	serv_addr.sin_port = htons(portno);

	/* Bind address to the socket */
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(EXIT_FAILURE);
	}

	/* Listen on socket - means we're ready to accept connections -
	 incoming connection requests will be queued */
	listen(sockfd, 5);
	printf("Server: Listening on port %d\n", portno);
	clilen = sizeof(cli_addr);

	while(1) {
		/* Accept a connection - block until a connection is ready to
		 be accepted. Get back a new file descriptor to communicate on. */
		clientsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
		if (clientsockfd < 0) {
	   		perror("ERROR on accept");
	   		exit(EXIT_FAILURE);
		}

		/* Create the arguments to give to the thread */
		ThreadArgs* thread_args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
		assert(thread_args);
		thread_args->clientsockfd = clientsockfd;
		thread_args->cli_addr = &cli_addr;
		thread_args->webRoot = webRoot;

		/* Start a new thread to handle the connection.*/
		if (pthread_create(&thread_id, NULL,  conn_handler,
								(void*)thread_args) != 0) {

				perror("could not create thread");
				return EXIT_FAILURE;
	   }
	   /* Merge the thread before termination */
	   pthread_join(thread_id , NULL);
	   free(thread_args);

   }

	/* Close the socket before exiting */
	close(sockfd);

	return 0;
}

/* ************************************************************************* */
