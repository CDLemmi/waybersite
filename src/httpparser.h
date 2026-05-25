#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include <stdint.h>

typedef enum {UNKNOWN, GET} RequestMethod;

typedef struct HTTPRequest {
	RequestMethod request_method;
	char path[1024];
	char host[1024];
	char session_cookie[256];
	int connection_keep_alive;
} HTTPRequest;


HTTPRequest parse_http_request(char* s);



#endif
