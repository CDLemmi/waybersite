#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>

typedef struct {
	int status_code;
	char content_type[1024];
	long content_length;
	char set_cookie[1024];
	char* content;
} HTTPResponse;

void http_response_set_content(HTTPResponse* r, char* content, int content_size);

void http_response_set_content_str(HTTPResponse* r, char* content);

void free_http_response(HTTPResponse r);

void write_http_response(HTTPResponse r, int fd);



typedef enum {UNKNOWN, GET, POST} RequestMethod;

typedef struct {
	RequestMethod request_method;
	char path[1024];
	char host[1024];
	char session_cookie[256];
	int connection_keep_alive;
	long content_length;
	char* content;
} HTTPRequest;


HTTPRequest parse_http_request(char* s);

void free_http_request(HTTPRequest r);

#endif
