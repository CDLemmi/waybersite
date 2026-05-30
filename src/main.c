

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <cjson/cJSON.h>

#include "http.h"
#include "db.h"
#include "users.h"

#define PORT 8080
#define BUFFER_SIZE 4096


//TODO implement
int verify_session_cookie(char* session_cookie) {
	return 0;
}

char* handle_api_request(char* json_request, char* api_endpoint, int user_id, int* error) {
	char* json_response = calloc(2048, sizeof(char));
	if(!strncmp(api_endpoint, "account_page", sizeof("account_page"))) {
		strcpy(json_response, "{\"username\": \"Albon\"}");
		return json_response;
	}

	if(!strncmp(api_endpoint, "user_change", sizeof("user_change"))) {
		
	}
	return NULL;
}






HTTPResponse handle_request(HTTPRequest request, Database db) {
	HTTPResponse response = {0};

	if(!strncmp(request.path, "/userapi/signin", sizeof("/userapi/signin"))) {
		cJSON* json = cJSON_ParseWithLength(request.content, request.content_length);
		char* name = cJSON_GetObjectItem(json, "username")->valuestring;
		char* password = cJSON_GetObjectItem(json, "password")->valuestring;
		char* session_id = handle_user_login(name, password, db);
		if(session_id == NULL) {
			response.status_code = 401;
		} else {
			response.status_code = 200;
			char set_cookie[1024] = "session=";
			strcat(set_cookie, session_id);
			free(session_id); 
			strcpy(response.set_cookie, set_cookie);
		}
		cJSON_Delete(json);
	} else if(!strncmp(request.path, "/api/", 5)) {
		int user_id = verify_session_cookie(request.session_cookie);
		if(user_id < 0) {
			response.status_code = 401;
		} else {
			int error = 0;
			char* json_response = handle_api_request(request.content, request.path + 5, user_id, &error);
			if(error) {
				response.status_code = 400;
			} else {
				response.status_code = 200;
			}
			http_response_set_content_str(&response, json_response);
			free(json_response);
			strcpy(response.content_type, "application/json");
		}
	} else {
		char path[256] = "web";
		strcat(path, request.path);
		char* ext = strrchr(path, '.') + 1;
		int ext_not_supported = 0;
		if(ext == NULL + 1) {
			ext_not_supported = 1;
		} else if(!strcmp(ext, "html")) {
			strcpy(response.content_type, "text/html; charset=utf-8");
		} else if(!strcmp(ext, "css")) {
			strcpy(response.content_type, "text/css");
		} else if(!strcmp(ext, "js")) {
			strcpy(response.content_type, "application/javasript");
		} else if(!strcmp(ext, ".jpg")) {
			strcpy(response.content_type, "image/jpeg");
		} else {
			ext_not_supported = 1;
		}
		if(strstr(request.path, "..") != NULL || access(path, R_OK) || ext_not_supported) {
			response.status_code = 404;
			memset(response.content_type, 0, sizeof(response.content_type));
			strcpy(response.content_type, "text/html");
			http_response_set_content_str(&response, "<html><body><h1>Error 404</h1>page not found</body></html>");
		} else {
			FILE* file = fopen(path, "rb");
			char content[16384];
			int content_size = fread(content, sizeof(char), 16384, file);
			http_response_set_content(&response, content, content_size);
			response.status_code = 200;
		}
		

	}

	return response;	
}


void handle_connection(int client_fd, Database db) {



	while(1) {
		char buffer[BUFFER_SIZE] = {0};
	
		read(client_fd, buffer, BUFFER_SIZE - 1);

		if(strlen(buffer) == 0) continue;
		printf("Request:\n%s\n", buffer);

		HTTPRequest request = parse_http_request(buffer);
		printf("method=%d;path=%s;host=%s;session=%s;connection=%d\n\n", request.request_method, request.path, request.host, request.session_cookie, request.connection_keep_alive);

		//if(strcmp(r.host, "www.waybersite.de")) break;
		
		HTTPResponse response = handle_request(request, db);

		write_http_response(response, client_fd);

		free_http_response(response);
		free_http_request(request);

		if(1 || !request.connection_keep_alive) {
			break;
		}
	}
	close(client_fd);
}




int main() {
	printf("Hello World!\n");

	Database db = init_database();
	
	create_user(0, "root", "test", db);

	int server_fd, client_fd;
	struct sockaddr_in addr;
	int opt = 1;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	//allow port reuse
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

	listen(server_fd, 10);
	printf("Listening on port %d\n", PORT);

	while(1) {
		client_fd = accept(server_fd, NULL, NULL);

		handle_connection(client_fd, db);
	
	}

	close(server_fd);


	printf("program finished");
	return 0;
}







