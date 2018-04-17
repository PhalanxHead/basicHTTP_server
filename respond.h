/* * * * * * *
 * Module for responding to HTTP Reqests
 *
 * Author:  Luke Hedt - 832153
 * Email:   lhedt@student.unimelb.edu.au
 * Date:    2018/04/13
 */

#ifndef RESPOND_H
#define RESPOND_H

#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

/*
 * String Structure -> Holds the length as well.
 */
typedef struct s_string {
	char* char_arr;
	long int len;
} s_String;

/*
 * ThreadArgs struct: Holds the necessary file values for the connection thread.
 */
typedef struct s_thread_args {
	int clientsockfd;
	struct sockaddr_in *cli_addr;
	char* webRoot;
} ThreadArgs;

/*
 * Handles the HTTP connection, request and response formation
 * Designed for use in a pthread.
 *********************************************************************
 * INPUT VARIABLES:
 *** void* thread_args: pointer to the thread arguments structure.
 */
void* conn_handler(void* thread_args);

#endif
