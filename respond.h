#ifndef RESPOND_H
#define RESPOND_H

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

#endif /* RESPOND_H */
