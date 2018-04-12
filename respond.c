/**
 * Author:  Luke Hedt - 832153
 * DateBegan:    2018/04/05
 * Name:    respond.c
 * Purpose: Contains all the code and logic for the HTTP handler.
 *
 * Comments:    Very Much Not Done
 */

#define BUFFSIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ************************************************************************* */

/*
 * Reads the requested file.
 * Based on code from
 * https://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
 */
char *readFile(char *filename) {
    /* Open the file, make sure it's there and find the length*/
    FILE *f = fopen(filename, "rt");
    /* If the file isn't there, return a 404 */
    if(f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    /* Make the buffer big enough to fit the entire file */
    char *buffer = (char *) malloc(length + 1);
    /* Add an end-of-string char, then read the file */
    buffer[length] = '\0';
    fread(buffer, 1, length, f);
    /* Close the file and gimme my data */
    fclose(f);
    return buffer;
}

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

/*
 * Concatonates 2 strings
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

char* respond(char* webRoot, char* request) {
    char* content;
    char* response;

    content = readFile(concat(webRoot, request));
    if(content == NULL) {
        return fourohfour();
    }

    response = "Response";

    return response;
}
