#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <json-c/json.h>

#define SERVER_PORT 8000
#define SERVER_IP "127.0.0.1"

void *receive(void *arg) {
    int fd = *(int *)arg;
    char buf[1024] = {0};

    while (1) {
        int ret = recv(fd, buf, sizeof(buf) - 1, 0);
        if (ret <= 0) {
            perror("Receive error");
            break;
        }
        buf[ret] = '\0';
        printf("Received: %s\n", buf);
    }

    close(fd);
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Bind information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    ret = listen(sockfd, 10);
    if (ret == -1) {
        perror("Listen failed");
        exit(1);
    }

    // Receive the connection
    printf("Waiting for client to connect...\n");
    socklen_t length = sizeof(client_addr);
    int fd = accept(sockfd, (struct sockaddr *)&client_addr, &length);
    if (fd == -1) {
        perror("Accept failed");
        exit(1);
    }
    printf("Successfully connected to client %d\n", fd);

    close(fd);  // Close the TCP connection
    close(sockfd);  // Close socket

    return 0;
}
