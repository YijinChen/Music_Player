#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

//gcc test_app.c -o test_app -ljson-c


void *receive(void *arg){
    int sockfd = *(int *)arg;
    char buf[1024] = {0};
    while(1){
        recv(sockfd, buf, sizeof(buf), 0);
        printf("reveiced reply from server: %s\n", buf);
        memset(buf, 0, sizeof(buf));
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
    server_addr.sin_port = 8000;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    size_t ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1){
        perror("connect");
        exit(1);
    }

    char buf[1024] = {0};
    //check if app already binded
    const char *bl = "{\"cmd\": \"search_bind\", \"appid\": \"101\"}";
    send(sockfd, bl, strlen(bl), 0);
    recv(sockfd, buf, sizeof(buf), 0);

    //parse json
    struct json_object *obj = json_tokener_parse(buf);
    struct json_object *json;
    json_object_object_get_ex(obj, "result", &json);
    if(!strcmp(json_object_get_string(json), "yes")){
        printf("app already bound\n");
    }
    else if(!strcmp(json_object_get_string(json), "no")){
        printf("app not yet bound\n");

        //bind app with device
        const char *b2 = "{\"cmd\": \"bind\", \"appid\": \"101\", \"deviceid\": \"001\"}";
        ret = send(sockfd, b2, strlen(b2), 0);
        if (ret == -1){
            perror("send");
            exit(1);
        }
    }

    pthread_t tid; //use for receive info
    pthread_create(&tid, NULL, receive, &sockfd);

    sleep(5); //sleep for a time as long as the "keep alive" intervel
    const char *b = "{\"cmd\": \"app_start\"}";
    ret = send(sockfd, b, strlen(b), 0);
    if (ret == -1){
        perror("send");
        exit(1);
    }
    sleep(50000);

    close(sockfd);
}