

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "httpparser.h"

#define PORT 8080
#define BUFFER_SIZE 4096





void handle_client(int client_fd) {
	char buffer[BUFFER_SIZE] = {0};
	
	read(client_fd, buffer, BUFFER_SIZE - 1);

	printf("Request:\n%s\n", buffer);

	HTTPRequest r = parse_http_request(buffer);
	printf("method=%d;path=%s;host=%s;session=%s;connection=%d\n\n", r.request_method, r.path, r.host, r.session_cookie, r.connection_keep_alive);

	char* response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text\r\n"
		"Connection: close\r\n"
		"\r\n"
		"<html><body><h1>Hello Wayber!</h1>deploy test 1 successful</body></html>";



	write(client_fd, response, strlen(response));
	close(client_fd);
}




int main() {
	printf("Hello World!\n");
	
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

		handle_client(client_fd);
	
	}

	close(server_fd);


	printf("program finished");
	return 0;
}







