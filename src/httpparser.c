#include "httpparser.h"

#include <string.h>








HTTPRequest parse_http_request(char* s) {
	HTTPRequest r;

	char method_s[16] = {0};
	int method_end = strchr(s, ' ') - s;
	strncpy(method_s, s, method_end);
	if(!strcmp(method_s, "GET")) {
		r.request_method = GET;
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
			char pair_s[512] = {0};
			strcpy(pair_s, value);
			while(1) {
				char* pair_end = strchr(pair_s, ';');
				if(pair_end == NULL) pair_end = pair_s + strlen(pair_s);
				char* equals_pos = strchr(pair_s, '=');
				char cookie_name[256];
				strncpy(cookie_name, pair_s, equals_pos - pair_s);
				char cookie_value[256];
				strncpy(cookie_value, equals_pos + 1, pair_end - equals_pos - 1);
				if(!strcmp(cookie_name, "session")) {
					strcpy(r.session_cookie, cookie_value);
				}
				if(*pair_end == ';') {
					while(*(++pair_end) == ' ');
					strcpy(pair_s, pair_end);
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
		}
		line_s = line_end + 2;
	}

	return r;


}




