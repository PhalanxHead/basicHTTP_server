/**
 * Author:  Luke Hedt - 832153
 * DateBegan:    2018/04/05
 * Name:    respond.c
 * Purpose: Contains all the code and logic for the HTTP handler.
 *
 */

#define BUFFSIZE 4096
#define SHORTBUFF 256
#define STARTFILE 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* ************************************************************************* */

/*
 * Concats 2 strings
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

/*
 * Reads the requested file.
 * Based on code from
 * https://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
 */
char *readFile(char* filename) {
    /* Make sure we're not reading a folder file */
    if(filename[strlen(filename)-1] == '/') {
        return NULL;
    }
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
    char* response;
    response = "\n"
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

    char* header = "HTTP/1.0 404 Not Found\n";
    char* lenhdr = sprintf("Content-Length: %d\n", strlen(response));
    char* conhdr = "Connection: close\n";
    char* typehdr = "Content-Type: text/html\n";

    char curtime[SHORTBUFF];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(curtime, sizeof(curtime), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    char* timehdr = concat("Date: ", curtime);
    timehdr = concat(timehdr, "\n");

    printf("%s\n\n", timehdr);

    return response;
}

/* ************************************************************************* */

/*
 * Forms the response to the HTTP Request
 */
char* respond(char* webRoot, char* request) {
    char* content;
    char* response;
    char* httpReq = concat(webRoot, request);

    /* Gets the file */
    printf("Getting File %s\n", httpReq);
    content = readFile(httpReq);
    if(content == NULL) {
        printf("File \"%s\" does not exist or is not valid. Sending 404.\n\n",
                httpReq);
        return fourohfour();
    }

    /* Adds HTTP Headers */
    response = content;

    return response;
}

/* ************************************************************************* */

/*
 * Parse's the HTTP request and finds the requested file string.
 */
char* parseRequest(char* httpReq) {
    char* fileReq;
    int readChar = STARTFILE;
    int writeChar = 0;

    /* Open a request buffer and zero it */
    fileReq = (char*)malloc(SHORTBUFF*sizeof(char));
    bzero(&fileReq, strlen(fileReq));

    while(httpReq[readChar] != ' ') {
        fileReq[writeChar] = httpReq[readChar];
        writeChar++;
        readChar++;
    }
    /* Add the String Terminator */
    fileReq[writeChar] = '\0';

    return fileReq;
}
