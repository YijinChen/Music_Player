#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <json-c/json.h>
#include <pthread.h>


void *receive(void *arg){
    int fd = *(int *)arg;
    

    while(1){
        char buf[1024] = {0};
        int ret = recv(fd, buf, sizeof(buf), 0);
        if(ret == -1){
            perror("recv");
        }

        struct json_object *json = json_tokener_parse(buf);
        struct json_object *obj;
        json_object_object_get_ex(json, "cmd", &obj);

        char cmd[32] = {0};
        strcpy(cmd, json_object_get_string(obj));

        if(!strcmp(cmd, "reply")){
            printf("operate successfully\n");
            printf("%s\n", buf);
        }
        else if(!strcmp(cmd, "info")){
            printf("%s\n", buf);
        }
        else if(!strcmp(cmd, "reply_status")){
            printf("operate successfully\n");
            printf("%s\n", buf);
        }
        else if(!strcmp(cmd, "reply_music")){
            printf("operate successfully\n");
            printf("%s\n", buf);
        }
    }
}

int main(){
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    //create socket
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("socket");
        exit(1);
    }

    //bind information
    bzero(&server_addr, sizeof(server_addr)); //empty the struct variable
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = 8000; //port numer, 1024-65535
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //127.0.0.1 replace the host itself
    int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1){
        perror("bind");
        exit(1);
    }

    //listen for connection
    ret = listen(sockfd, 10);
    if(ret == -1){
        perror("listen");
        exit(1);
    }

    //receive the connection
    printf("waiting for client to connect ..\n");
    socklen_t length = sizeof(client_addr);
    //int length = sizeof(struct sockaddr);
    int fd = accept(sockfd, (struct sockaddr *)&client_addr, &length);
    if(fd == -1){
        perror("accept");
        exit(1);
    }
    printf("successfully connect to client %d\n", fd);

    //create a pthread for receiving info from client
    pthread_t tid;
    ret = pthread_create(&tid, NULL, receive, &fd);

    char buf[1024] = {0};
    //for sending message to client
    while(1){
        scanf("%s", buf);
        struct json_object *json =  json_object_new_object();
        json_object_object_add(json, "cmd", json_object_new_string(buf));
        const char *s = json_object_to_json_string(json);
        ret = send(fd, s, strlen(s), 0);

        // ret = recv(fd, buf, sizeof(buf), 0);
        // if(ret == -1){
        //     perror("recv");
        // }
        // if(!strcmp(buf, "bye")){
        //     break;
        // }
        // printf("%s\n", buf);
        // bzero(buf, sizeof(buf));
    }
    
    close(fd); //close the tcp connection
    close(sockfd); //close socket

    return 0;
}

