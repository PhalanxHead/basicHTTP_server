/* * * * * * *
 * Module for responding to HTTP Reqests
 *
 * Author:  Luke Hedt - 832153
 * Email:   lhedt@student.unimelb.edu.au
 * Date:    2018/04/13
 */

#ifndef RESPOND_H
#define RESPOND_H

/* Generates valid HTTP response
 * Inputs:
    *   webRoot - The root directory of the server_loop
    *   request - The bitstream from the recv call
 * Returns:
    *   Valid HTTP Response (404 if file not found.)
 */
char* respond(char* webRoot, char* request);

/* Searches request for reuqested file
 * Inputs:
    *   httpReq - The recieved request from recv
 * Returns:
    *   FILE_TO_READ's path
 */
char* parseRequest(char* httpReq);


#endif
