// i.MX6ULL Client (client_recv.c)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define SERVER_IP "18.185.92.160"  // Replace with Ubuntu server's public IP
int sockfd;
struct sockaddr_in server_addr;

void *connect_cb(void *arg){
    int count = 10;

    while(count--){
        //when sending connection request to server, let the first led shine
        //led_on(0);
        printf("Try to connect, left trial times = %d\n", count);
        //pthread_mutex_lock(&sock_mutex);
        int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        //pthread_mutex_unlock(&sock_mutex);
        if (ret == -1){
            printf("Failed to connect, next trial in 5 seconds...\n");
            //printf("Retrying connection in 5 seconds...\n");
            my_sleep(5); // If fail to connect, sleep 5 seconds
            //printf("finish sleep\n");
            continue;
        }
        else{
            printf("Successfully connect to server\n");
            break;
        }
    }

    if (count <= 0) {
        printf("Failed to connect to server after multiple attempts.\n");
        close(sockfd); // Close socket if connection fails
        return NULL;
    }

    //sucessfully connect to server, add fd to set
    //FD_SET(sockfd, &readfd);
    
    return NULL;
}

int InitSocket(){
    //create a thread and request the connection to server
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, connect_cb, NULL);
    //printf("ret: %d\n", ret);
    if (ret != 0){
        printf("Fail to create pthread\n");
        return 0;
    }
    // Wait for the connection thread to finish
    //pthread_join(tid, NULL);
    return 1;
}

int main() {
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

    InitSocket();

    // Connect to the server
    // if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    //     perror("Connection failed");
    //     close(sockfd);
    //     exit(EXIT_FAILURE);
    // }

    //printf("Connected to the server. Waiting for message...\n");

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
