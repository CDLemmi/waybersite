

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
#include "game.h"

#define PORT 8080
#define BUFFER_SIZE 4096





//error = 0 -> false
char* handle_api_request(cJSON* request_body, char* api_endpoint, User user, int* error, Database db) {
	if(!strcmp(api_endpoint, "account_page")) {
		cJSON* json = cJSON_CreateObject();
		cJSON_AddItemToObject(json, "username", cJSON_CreateString(user.name));
		cJSON_AddNumberToObject(json, "admin", user.admin);
		char* json_response = cJSON_Print(json);
		cJSON_Delete(json);
		return json_response;
	} else if(!strcmp(api_endpoint, "username-change")) {
		char* name = cJSON_GetObjectItem(request_body, "username_new")->valuestring;
		memset(user.name, 0, sizeof(user.name));
		strcpy(user.name, name);
		*error = user_save(user, db);
	} else if(!strcmp(api_endpoint, "dashboard-page")) {
		if(user.admin == 0) {
			*error = 2;
			return NULL;
		}
		int ids[256];
		int id_count;
		db_get_user_list(db, ids, &id_count);
		cJSON* json = cJSON_CreateObject();
		cJSON_AddItemToObject(json, "username", cJSON_CreateString(user.name));
		cJSON* user_list = cJSON_CreateArray();
		for(int i = 0; i < id_count; i++) {
			cJSON* user_json = cJSON_CreateObject();
			User user;
			user_get(&user, ids[i], db);
			cJSON_AddItemToObject(user_json, "id", cJSON_CreateNumber(user.id));
			cJSON_AddItemToObject(user_json, "username", cJSON_CreateString(user.name));
			cJSON_AddItemToObject(user_json, "admin", cJSON_CreateNumber(user.admin));
			cJSON_AddItemToArray(user_list, user_json);
		}
		cJSON_AddItemToObject(json, "user_list", user_list);
		char* json_response = cJSON_Print(json);
		cJSON_Delete(json);
		return json_response;
	} else if(!strcmp(api_endpoint, "user-add")) {
		if(user.admin == 0) {
			*error = 2;
			return NULL;
		}
		User user;
		char* name = cJSON_GetObjectItem(request_body, "username_new")->valuestring;
		char* password = cJSON_GetObjectItem(request_body, "password")->valuestring;
		int i;
		user_create(name, password, &i, db);
	} else if(!strcmp(api_endpoint, "user-remove")) {
		if(user.admin == 0) {
			*error = 2;
			return NULL;
		}
		int id = cJSON_GetObjectItem(request_body, "user_id")->valueint;
		db_delete_user(db, id);
	} else if(!strcmp(api_endpoint, "user-set-name")) {
		if(user.admin == 0) {
			*error = 2;
			return NULL;
		}
		User user;
		int id = cJSON_GetObjectItem(request_body, "user_id")->valueint;
		char* name = cJSON_GetObjectItem(request_body, "username_new")->valuestring;
		user_get(&user, id, db);
		strcpy(user.name, name);
		user_save(user, db);
	} else if(!strcmp(api_endpoint, "user-set-password")) {
		if(user.admin == 0) {
			*error = 2;
			return NULL;
		}
		User user;
		int id = cJSON_GetObjectItem(request_body, "user_id")->valueint;
		char* pw = cJSON_GetObjectItem(request_body, "password_new")->valuestring;
		char hash[HASH_SIZE];
		hash_password(hash, pw);
		user_get(&user, id, db);
		memcpy(user.hash, hash, HASH_SIZE);
		user_save(user, db);
	} else if(!strcmp(api_endpoint, "user-set-admin")) {
		if(user.admin == 0) {
			*error = 2;
			return NULL;
		}
		User user;
		int id = cJSON_GetObjectItem(request_body, "user_id")->valueint;
		int admin = cJSON_GetObjectItem(request_body, "admin")->valueint;
		user_get(&user, id, db);
		user.admin = admin;
		user_save(user, db);
	} else if(!strcmp(api_endpoint, "match-page")) {
		cJSON* json = cJSON_CreateObject();
		cJSON_AddItemToObject(json, "matches", get_match_list(user.id, db));
		char* s = cJSON_Print(json);
		return s;
	} else {
		*error = 1;
		char* errMes = calloc(2048,1);
		strcpy(errMes, "{\"errorMes\": \"Unused API endpoint\"}");
	}

	return NULL;
}






HTTPResponse handle_request(HTTPRequest request, Database db) {
	HTTPResponse response = {0};

	if(!strcmp(request.path, "/userapi/signin")) {
		cJSON* json = cJSON_ParseWithLength(request.content, request.content_length);
		cJSON* body = cJSON_GetObjectItem(json, "body");
		char* name = cJSON_GetObjectItem(body, "username")->valuestring;
		char* password = cJSON_GetObjectItem(body, "password")->valuestring;
		char* session_id = handle_user_login(name, password, db);
		if(session_id == NULL) {
			response.status_code = 401;
		} else {
			response.status_code = 200;
			char set_cookie[1024] = "session=";
			strcat(set_cookie, session_id);
			strcat(set_cookie, ";path=/;");
			free(session_id); 
			strcpy(response.set_cookie, set_cookie);
		}
		cJSON_Delete(json);
	} else if(!strcmp(request.path, "/userapi/changepw")) {
		cJSON* json = cJSON_ParseWithLength(request.content, request.content_length);
		cJSON* body = cJSON_GetObjectItem(json, "body");
		char* name = cJSON_GetObjectItem(body, "username")->valuestring;
		char* pw_old = cJSON_GetObjectItem(body, "pw_old")->valuestring;
		char* pw_new = cJSON_GetObjectItem(body, "pw_new")->valuestring;
		if(change_password(name, pw_old, pw_new, db)) {
			response.status_code = 401;
		} else {
			response.status_code = 200;
		}
		cJSON_Delete(json);
	} else if(!strcmp(request.path, "/")) {
		response.status_code = 303;
		strcpy(response.location, "/home.html");
	} else if(!strncmp(request.path, "/api/", 5)) {
		int user_id = db_verify_user_session(db, request.session_cookie);
		if(user_id < 0) {
			response.status_code = 401;
		} else {
			int error = 0;
			cJSON* json = cJSON_ParseWithLength(request.content, request.content_length);
			cJSON* body = cJSON_GetObjectItem(json, "body");

			User user;
			user_get(&user, user_id, db);
			printf("[INFO] verified user %s(id=%d)\n", user.name, user_id);
			char* json_response = handle_api_request(body, request.path + 5, user, &error, db);
			cJSON_Delete(body);
			if(error == 2) {
				response.status_code = 401;
			} else if(error) {
				response.status_code = 400;
			} else {
				response.status_code = 200;
			}

			if(json_response != NULL)
			{
			    http_response_set_content_str(&response, json_response);
				free(json_response);
			}

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
		} else if(!strcmp(ext, "jpg")) {
			strcpy(response.content_type, "image/jpeg");
		} else if(!strcmp(ext, "json")) {
			strcpy(response.content_type, "application/json");
		} else {
			ext_not_supported = 1;
		}

		if(strstr(request.path, "..") != NULL || access(path, R_OK) || ext_not_supported) {
			printf("[INFO] refusing file path: %s\n", path); 
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
			fclose(file);
		}
		

	}

	return response;	
}


void handle_connection(int client_fd, Database db) {



	char buffer[BUFFER_SIZE] = {0};
	
	read(client_fd, buffer, BUFFER_SIZE - 1);
	if(strlen(buffer) != 0) {
		HTTPRequest request = parse_http_request(buffer);
		char method[16];
		if(request.request_method == GET) {
			strcpy(method, "GET");
		} else if(request.request_method == POST) {
			strcpy(method, "POST");
		} else {
			strcpy(method, "UNKNOWN");
		}
		printf("Request: %s %s\n", method, request.path);
	
		HTTPResponse response = handle_request(request, db);
		printf("Response: %d\n", response.status_code);
		write_http_response(response, client_fd);
		free_http_response(response);
		free_http_request(request);
	}
	close(client_fd);
}




int main() {
	printf("Hello World!\n");

	Database db = init_database();
	
	user_create( "root", "test", 0, db);

	init_game(db);

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







