/**
 * Author:  Luke Hedt - 832153
 * Date:    2018/04/05
 * Name:    respond.c
 * Purpose: Contains all the code and logic for the HTTP handler.
 *
 * Comments:    Very Much Not Done
 */

#define BUFFSIZE 4096

#include<stdio.h>
#include<stdlib.h>

/* ************************************************************************* */

/*
 * Generates a 404 message and returns it.
 */
char* fourohfour() {
    char* response = (char*)malloc(sizeof(char[BUFFSIZE]));
    response = "HTTP/1.0 404 Not Found\n"
    "Content-Length: 330\n"
    "Connection: close\n"
    "Content-Type: text/html\n"
    "\n"
    "<!DOCTYPE html>\n"
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
        "<p>We couldn't find the file you requested. Perhaps there's a typo in your URL?</p>\n"
        "</div>\n"
    "</body></html>\n";

    return response;
}

/* ************************************************************************* */

char* respond(char* webRoot, char* request) {
    char* response;

    response = fourohfour();

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
