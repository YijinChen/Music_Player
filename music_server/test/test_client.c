#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

//gcc test_client.c -o test_client

void *receive(void *arg){
    int sockfd = *(int *)arg;
    char buf[1024] = {0};
    while(1){
        memset(buf, 0, sizeof(buf));
        int ret = recv(sockfd, buf, sizeof(buf), 0);
        printf("received info from server: %s\n", buf);
        sleep(1);
        const char *s = "{\"cmd\": \"reply\", \"result\": \"success\"}";
        send(sockfd, s, strlen(s), 0);
    }
}

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("socket");
        exit(1);    
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1){
        perror("connect");
        exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, receive, &sockfd);

    while(1){
        const char *buf = "{\"cmd\": \"info\", \"status\": \"alive\", \"deviceid\": \"001\"}";
        ret = send(sockfd, buf, strlen(buf), 0);
        if (ret == -1){
            perror("send");
            exit(1);
        }
        sleep(5);
    }
    close(sockfd);
}