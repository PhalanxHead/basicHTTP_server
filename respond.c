/**
 * Author:          Luke Hedt - 832153
 * Email/Login ID:  lhedt@student.unimelb.edu.au
 * DateBegan:       2018/04/05
 * Name:            respond.c
 * Purpose: Contains all the code and logic for the HTTP handler.
 *
 */

#define BUFFSIZE 4096
#define SHORTBUFF 256
#define STARTFILE 4
#define OOFFSET 1

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
    char *result = malloc(strlen(s1)+strlen(s2)+OOFFSET);
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
    char *buffer = (char *) malloc(length + OOFFSET);
    /* Add an end-of-string char, then read the file */
    buffer[length] = '\0';
    fread(buffer, 1, length, f);
    /* Close the file and gimme my data */
    fclose(f);
    return buffer;
}

/* ************************************************************************* */


/*
 * Gets the current time for the Time Header
 * Code Based on pmg's response:
 * https://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c
 */
char* getCurrTime() {
    char* curtime = (char*)malloc(SHORTBUFF*sizeof(char));
    time_t now = time(0);
    struct tm tm = *gmtime(&now);

    strftime(curtime, sizeof(curtime), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    return curtime;
}

/* ************************************************************************* */

/*
 * Generates a 404 message and returns it.
 * Page is hardcoded for sake of simplicity & sake of remote automated testing
 *  No other good reason not to have a nicer 404 page which is loaded.
 */
char* fourohfour() {
    char* response;
    /* 404 Page Content */
    response = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
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


    /* Generate Headers */
    char* header = "HTTP/1.0 404 Not Found\n";
    char* lenhdr = (char*)malloc(SHORTBUFF*sizeof(char));
    /* Remove 1 for string terminator (Because Webkit is evil) */
    sprintf(lenhdr, "Content-Length: %d\n", (int)strlen(response));
    char* conhdr = "Connection: Closed\n";
    char* typehdr = "Content-Type: text/html\n";
    char* timehdr = concat("Date: ", getCurrTime());
    timehdr = concat(timehdr, "\n");

    /* Join all of the individual headers */
    response = concat("\n", response);
    response = concat(typehdr, response);
    response = concat(conhdr, response);
    response = concat(lenhdr, response);
    response = concat(timehdr, response);
    response = concat(header, response);

    return response;
}

/* ************************************************************************* */


/*
 * Returns the MIME type of the requested file.
 */
char* getMimeType(char* request) {
    char* mtype = (char*)malloc(SHORTBUFF*sizeof(char));

    /* Get the last '.' in the filename */
    char* ext = strrchr(request, '.');
    if (!ext) {
        return NULL;
    }

    /* Work out it's type and return the MIME type */

    return ext + 1;
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

    /* Generate Headers */
    char* header = "HTTP/1.0 200 OK\n";
    char* lenhdr = (char*)malloc(SHORTBUFF*sizeof(char));
    /* Remove 1 for string terminator (Because Webkit is evil) */
    sprintf(lenhdr, "Content-Length: %d\n", (int)strlen(content));
    char* conhdr = "Connection: Closed\n";
    char* typehdr = (char*)malloc(SHORTBUFF*sizeof(char));
    sprintf(typehdr, "Content-Type: %s\n", getMimeType(request));
    char* timehdr = concat("Date: ", getCurrTime());
    timehdr = concat(timehdr, "\n");

    /* Join all of the individual headers */
    response = concat("\n", content);
    //response = concat(typehdr, content);
    response = concat(conhdr, response);
    response = concat(lenhdr, response);
    response = concat(timehdr, response);
    response = concat(header, response);

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

    /* Get the requested file & path */
    while(httpReq[readChar] != ' ') {
        fileReq[writeChar] = httpReq[readChar];
        writeChar++;
        readChar++;
    }
    /* Add the String Terminator */
    fileReq[writeChar] = '\0';

    return fileReq;
}
