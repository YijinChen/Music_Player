#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define SERVER_PORT 8000
#define SERVER_IP "127.0.0.1"

int g_sockfd;
fd_set readfd;

void my_sleep(int seconds) {
    sleep(seconds);
}

void led_on(int led) {
    printf("LED %d is on\n", led);
}

void send_server(int signo) {
    printf("Sending data to server...\n");
}

void *connect_cb(void *arg) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        printf("Trying to connect...\n");
        int ret = connect(g_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret == -1) {
            perror("Failed to connect");
            printf("Retrying connection in 5 seconds...\n");
            my_sleep(5); // If fail to connect, sleep 5 seconds
            continue;
        }

        // When connection is successful, let all 4 LEDs shine
        led_on(0);
        led_on(1);
        led_on(2);
        led_on(3);

        // After 5 seconds, send SIGALRM to process
        alarm(5);
        signal(SIGALRM, send_server);

        // Successfully connect to server, add fd to set
        FD_SET(g_sockfd, &readfd);
        break;
    }
    return NULL;
}

int main() {
    g_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    pthread_t tid;
    pthread_create(&tid, NULL, connect_cb, NULL);
    pthread_join(tid, NULL);  // Wait for the connection to be established

    // // Main loop or additional code can go here
    // char buffer[1024];
    // while (1) {
    //     printf("Enter message: ");
    //     fgets(buffer, sizeof(buffer), stdin);
    //     send(g_sockfd, buffer, strlen(buffer), 0);
    // }

    close(g_sockfd);
    return 0;
}
