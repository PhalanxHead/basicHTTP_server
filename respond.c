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

char* respond(char* webRoot, char* request) {
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
    "<h1>404: File Not Found</h1>\n"
    "<p>The requested URL was not found on this server.</p>\n"
    "</body></html>\n";

    return response;
}

/*
fd = open(FILE_TO_SEND, O_RDONLY);
       if (fd == -1)
       {
               fprintf(stderr, "Error opening file --> %s", strerror(errno));

               exit(EXIT_FAILURE);
       }

       // Get file stats
       if (fstat(fd, &file_stat) < 0)
       {
               fprintf(stderr, "Error fstat --> %s", strerror(errno));

               exit(EXIT_FAILURE);
       }

       fprintf(stdout, "File Size: \n%d bytes\n", file_stat.st_size);
*/
