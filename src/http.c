#include "http.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>



void http_response_set_content(HTTPResponse* r, char* content, int content_size) {
	r->content_length = content_size;
	r->content = calloc(content_size, sizeof(char));
	memcpy(r->content, content, content_size);
}

void http_response_set_content_str(HTTPResponse* r, char* content) {
	int length = strlen(content);
	r->content_length = length;
	r->content = calloc(length + 1, sizeof(char));
	memcpy(r->content, content, length);
}

void parse_http_response(HTTPResponse r) {
	free(r.content);
}

void write_http_response(HTTPResponse r, int fd) {
	dprintf(fd, "HTTP/1.1 ");
	if(r.status_code == 200) {
		dprintf(fd, "200 OK\r\n");
	} else if(r.status_code == 404) {
		dprintf(fd, "404 Not Found\r\n");
	} else if(r.status_code == 401) {
		dprintf(fd, "401 Unauthorized\r\n");
	} else if(r.status_code == 400) {
		dprintf(fd, "400 Bad Request\r\n");
	}
	if(r.content_length > 0) {
		dprintf(fd, "Content-Type: %s\r\n", r.content_type);
		dprintf(fd, "CacheControl: no-store\r\n");
	}

	if(strlen(r.set_cookie) > 0) {
		dprintf(fd, "Set-Cookie: %s\r\n", r.set_cookie);
	}
	dprintf(fd, "Content-Length: %ld\r\n\r\n", r.content_length);
	 
	write(fd, r.content, r.content_length);
}




HTTPRequest parse_http_request(char* s) {
	HTTPRequest r;

	char method_s[16] = {0};
	int method_end = strchr(s, ' ') - s;
	strncpy(method_s, s, method_end);
	if(!strcmp(method_s, "GET")) {
		r.request_method = GET;
	} else if(!strcmp(method_s, "POST")) {
		r.request_method = POST;
	} else {
		r.request_method = UNKNOWN;
	}

	char* path_start = s + method_end + 1;
	int path_length = strchr(path_start, ' ') - path_start;
	strncpy(r.path, path_start, path_length);

	char* line_s = strstr(s, "\r\n") + 2;
	while(1) {
		char* line_end = strstr(line_s, "\r\n");
		if(line_end == line_s) break;
		int colon_pos = strchr(line_s, ':') - line_s;
		char name[64] = {0};
		strncpy(name, line_s, colon_pos);
		char* value_s = line_s + colon_pos;
		while(*(++value_s) == ' ');
		char value[512] = {0};
		strncpy(value, value_s, line_end - value_s);

		if(!strcmp(name, "Host")) {
			strcpy(r.host, value);
		} else if(!strcmp(name, "Cookie")) {
			char* pair_s = value;
			while(1) {
				char* pair_end = strchr(pair_s, ';');
				if(pair_end == NULL) pair_end = pair_s + strlen(pair_s);
				char* equals_pos = strchr(pair_s, '=');
				char cookie_name[256] = {0};
				strncpy(cookie_name, pair_s, equals_pos - pair_s);
				char cookie_value[256] = {0};
				strncpy(cookie_value, equals_pos + 1, pair_end - equals_pos - 1);
				if(!strcmp(cookie_name, "session")) {
					strcpy(r.session_cookie, cookie_value);
				}
				if(*pair_end == ';') {
					while(*(++pair_end) == ' ');
					pair_s = pair_end;
				} else {
					break;
				}
			}
		} else if(!strcmp(name, "Connection")) {
			if(!strcmp(value, "keep-alive")) {
				r.connection_keep_alive = 1;
			} else {
				r.connection_keep_alive = 0;
			}
		} else if(!strcmp(name, "Content-Length")) {
			r.content_length = strtol(value, NULL, 10);
		}
		line_s = line_end + 2;
	}
	
	if(r.content_length != 0) {
		r.content = calloc(r.content_length + 1, sizeof(char));
		strncpy(r.content, line_s + 2, r.content_length);
	}

	return r;
}


void free_http_response(HTTPResponse r) {
	if(r.content_length >= 0) {
		free(r.content);
	}
}

void free_http_request(HTTPRequest r) {
	if(r.content_length >= 0) {
		free(r.content);
	}
}



