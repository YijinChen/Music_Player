#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("socket");
        exit(1);    
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = 8000;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    size_t ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1){
        perror("connect");
        exit(1);
    }

    while(1){
        const char *buf = "{\"cmd\": \"bind\", \"appid\": \"101\", \"deviceid\": \"001\"}";
        ret = send(sockfd, buf, strlen(buf), 0);
        if (ret == -1){
            perror("send");
            exit(1);
        }
        sleep(5); //sleep at least for a time as long as the "keep alive" intervel
        const char *b = "{\"cmd\": \"app_start\"}";
        ret = send(sockfd, b, strlen(b), 0);
        if (ret == -1){
            perror("send");
            exit(1);
        }
        sleep(50000);
    }
    close(sockfd);
}