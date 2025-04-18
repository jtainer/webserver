#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "endpoints.h"

#define PORT 8080
#define BACKLOG 10
#define BUFFER_SIZE 4096

static volatile int should_exit = 0;
void signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGTERM) {
		should_exit = 1;
	}
}

const char* get_file_ext(const char* filename) {
	const char* dot = strrchr(filename, '.');
	if (dot == NULL)
		return "";
	else
		return dot + 1;
}

char rx_buffer[BUFFER_SIZE];
char tx_buffer[BUFFER_SIZE];

// Function to send the HTML file to the client
void send_html_file(int client_socket, const char* filepath) {
	// Open the HTML file
	FILE* html_file = fopen(filepath, "r");
	if (!html_file) {
		perror("Could not open HTML file");
		return;
	}

	// Prepare the HTTP response header
	char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	send(client_socket, header, strlen(header), 0);

	// Read the HTML file and send it to the client
	while (fgets(tx_buffer, BUFFER_SIZE, html_file)) {
		send(client_socket, tx_buffer, strlen(tx_buffer), 0);
	}

	fclose(html_file);
}

// Function to send an image file to the client
void send_image_file(int client_socket, const char* filepath) {
	// Open the image file
	FILE *image_file = fopen(filepath, "rb");
	if (!image_file) {
		perror("Could not open HTML file");
		return;
	}

	// Prepare the HTTP response header
	char header[] = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n";
	send(client_socket, header, strlen(header), 0);

	// Read the image file and send it to the client
	size_t bytes_read = 0;
	while ((bytes_read = fread(tx_buffer, 1, BUFFER_SIZE, image_file)) > 0) {
		send(client_socket, tx_buffer, bytes_read, 0);
	}

	fclose(image_file);
}

int main() {
	// Catch ctrl+c so we can close the socket before exiting
	signal(SIGINT, signal_handler);

	// Create endpoint resolution table
	Table endpoints = LoadEndpoints();

	// Create the server socket
	// Setting SOCK_NONBLOCK means we can poll accept() instead of blocking on it
	int server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server_socket == -1) {
		fprintf(stderr, "Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Configure server address struct
	struct sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	// Bind the socket to the specified port
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		fprintf(stderr, "Bind failed");
		close(server_socket);
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_socket, BACKLOG) == -1) {
		fprintf(stderr, "Listen failed");
		close(server_socket);
		exit(EXIT_FAILURE);
	}

	printf("Server is running on http://localhost:%d\n", PORT);

	// Accept and handle connections
	while (!should_exit) {
		struct sockaddr client_addr = { 0 };
		socklen_t addr_len = sizeof(client_addr);
		int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
		if (client_socket == -1) {
			//perror("Accept failed");
			continue;
		}

		ssize_t bytes_read = read(client_socket, rx_buffer, BUFFER_SIZE-1);
		if (bytes_read >= 0) {
			rx_buffer[bytes_read] = 0;
		}
		else {
			close(client_socket);
			continue;
		}

		char* token[2] = { 0 };
		token[0] = strtok(rx_buffer, " \t\r\n");
		token[1] = strtok(NULL, " \t\r\n");
		if (token[0] == NULL) {
			close(client_socket);
			continue;
		}
		else if (token[1] == NULL) {
			// Should send 404 I think
			close(client_socket);
			continue;
		}
		else if (strcmp(token[0], "GET")) {
			close(client_socket);
			continue;
		}
/*
		printf("GET %s\n", token[1]);
		if (!strcmp(token[1], "/")) {
			send_html_file(client_socket);
		}
		else if (!strcmp(token[1], "/image")) {
			send_image_file(client_socket);
		}
		else {
			// 404
		}
*/

		const char* filepath = ResolveEndpoint(&endpoints, token[1]);
		if (filepath) {
			const char* ext = get_file_ext(filepath);
			if (!strcmp(ext, "html"))
				send_html_file(client_socket, filepath);
			else if (!strcmp(ext, "png"))
				send_image_file(client_socket, filepath);
		}

		close(client_socket);
	}

	// Close the server socket
	close(server_socket);

	UnloadEndpoints(&endpoints);

	return 0;
}

