// i.MX6ULL Client (client_recv.c)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define SERVER_IP "18.185.92.160"  // Replace with Ubuntu server's public IP

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. Waiting for message...\n");

    // Receive message from server
    int ret = recv(sockfd, buffer, sizeof(buffer), 0);
    if (ret > 0) {
        buffer[ret] = '\0';  // Null-terminate the received data
        printf("Received message: %s\n", buffer);
    } else if (ret == 0) {
        printf("Connection closed by server.\n");
    } else {
        perror("recv");
    }

    // Close socket
    close(sockfd);
    return 0;
}
