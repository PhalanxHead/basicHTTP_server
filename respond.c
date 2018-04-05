/**
 * Author:  Luke Hedt - 832153
 * Date:    2018/04/05
 * Name:    respond.c
 * Purpose: Contains all the code and logic for the HTTP handler.
 *
 * Comments:    Very Much Not Done
 */


#include<stdio.h>
#include<stdlib.h>

char* respond() {
    char* response = (char*)malloc(sizeof(char[256]));
    response = "HTTP/1.0 404 Not Found\n"
    "Content-Length: 204\n"
    "Connection: close\n"
    "Content-Type: text/html\n"
    "\n"
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
    "<html><head>\n"
    "<title>404 Not Found</title>\n"
    "</head><body>\n"
    "<h1>Not Found</h1>\n"
    "<p>The requested URL /t.html was not found on this server.</p>\n"
    "</body></html>\n";

    return response;
}
