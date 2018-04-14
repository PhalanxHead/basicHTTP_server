/**
 * Author:          Luke Hedt - 832153
 * Email/Login ID:  lhedt@student.unimelb.edu.au
 * DateBegan:       2018/04/05
 * Name:            respond.c
 * Purpose: Contains all the code and logic for the HTTP handler.
 */

#define BUFFSIZE 4096
#define SHORTBUFF 256
#define STARTFILE 4
#define FOF_TRUE 1
#define FOF_FALSE 0

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
char* readFile(char* filename) {
    /* Make sure we're not reading a folder file */
    if(filename[strlen(filename)-1] == '/') {
        return NULL;
    }
    /* Open the file, make sure it's there and find the length*/
    FILE* f = fopen(filename, "rt");
    /* If the file isn't there, return a 404 */
    if(f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    /* Make the buffer big enough to fit the entire file */
    char* buffer = (char*)malloc(length + 1);
    assert(buffer);

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
    assert(curtime);
    time_t now = time(0);
    struct tm tm = *gmtime(&now);

    strftime(curtime, SHORTBUFF, "%a, %d %b %Y %H:%M:%S %Z\n", &tm);


    return curtime;
}


/* ************************************************************************* */

/*
 * Extremely primitive MIME Handler.
 * Returns the MIME type of the requested file.
 */
char* getMimeType(char* request) {

    /* Get the last '.' in the filename */
    char* ext = strrchr(request, '.');
    if (!ext) {
        return NULL;
    }

    /* Work out it's type and return the MIME type */
    if(strcmp(ext, ".html") == 0) {
        return "text/html\n";

    } else if(strcmp(ext, ".css") == 0) {
        return "text/css\n";

    } else if((strcmp(ext, ".jpg") == 0) || (strcmp(ext, ".jpeg") == 0)) {
        return "image/jpeg\n";

    } else if(strcmp(ext, ".js") ==  0) {
        return "application/javascript\n";
    }

    return NULL;
}

/* ************************************************************************* */

/*
 * Generates the required HTTP Headers.
 */
char* genHeaders(char* content, int contentLen, char* requestPath, int FoF) {
  char *mainhdr, *lenhdr, *conhdr, *typehdr, *timehdr;
  /* This way of defining response iterations is tiring but if I don't it leaks
    memory big time */
  char *response0, *response1, *response2, *response3;
  char *response4, *response5;
  char* curtime;

  /* Treat 404 Errors slightly differently due to them being Hardcoded */
  if(FoF == FOF_TRUE) {
    mainhdr = "HTTP/1.0 404 Not Found\n";
    typehdr = "Content-Type: text/html\n";

  } else {
    mainhdr = "HTTP/1.0 200 OK\n";
    typehdr = (char*)malloc(SHORTBUFF*sizeof(char));
    assert(typehdr);
    sprintf(typehdr, "Content-Type: %s\n", getMimeType(requestPath));
  }

  /* Standard Headers */
  lenhdr = (char*)malloc(SHORTBUFF*sizeof(char));
  assert(lenhdr);
  sprintf(lenhdr, "Content-Length: %d\n", contentLen);
  conhdr = "Connection: Closed\n";
  curtime = getCurrTime();
  timehdr = concat("Date: ", curtime);
  free(curtime);

  /* Join all of the individual headers and free the corresponding memory
    blocks */
  response0 = concat("\n", content);
  response1 = concat(typehdr, response0);
  free(response0);

  if(FoF == FOF_FALSE) {
    free(typehdr);
  }

  response2 = concat(conhdr, response1);
  free(response1);
  response3 = concat(lenhdr, response2);
  free(response2);
  free(lenhdr);
  response4 = concat(timehdr, response3);
  free(response3);
  free(timehdr);
  response5 = concat(mainhdr, response4);
  free(response4);

  return response5;

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


    response = genHeaders(response, (int)strlen(response), NULL, FOF_TRUE);

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
        free(httpReq);
        return fourohfour();
    }

    response = genHeaders(content, (int)strlen(content), request, FOF_FALSE);

    free(httpReq);
    free(content);

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

    /* Open a request buffer */
    fileReq = (char*)malloc(SHORTBUFF*sizeof(char));
    assert(fileReq);

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
