#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define RETRY_DELAY 2
#define MAX_RETRIES 5

int main() {
    int sock = 0, retries = 0;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        return -1;
    }

    while (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        if (++retries > MAX_RETRIES) {
            perror("Connection Failed");
            return -1;
        }
        printf("Retrying connection in %d seconds...\n", RETRY_DELAY);
        sleep(RETRY_DELAY);
    }

    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    close(sock);
    return 0;
}

