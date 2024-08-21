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
        // sleep(1);
        // const char *s = "{\"cmd\": \"reply\", \"result\": \"success\"}";
        // send(sockfd, s, strlen(s), 0);

        struct json_object *obj = json_tokener_parse(buf);
        struct json_object *json;
        json_object_object_get_ex(obj, "cmd", &json);
        if(!strcmp(json_object_get_string(json), "start")){
                printf("received [start]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"start_success\"}";
                ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "suspend")){
                printf("received [suspend]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"suspend_success\"}";
                ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "continue")){
                printf("received [continue]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"continue_success\"}";
                ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "prior")){
                printf("received [prior]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"prior_success\"}";
                ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "next")){
                printf("received [next]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"next_success\"}";
                ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "volume_up")){
                printf("received [volume_up]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"success\"}";
                int ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "volume_down")){
                printf("received [volume_down]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"success\"}";
                int ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "sequence")){
                printf("received [sequence]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"success\"}";
                int ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "random")){
                printf("received [random]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"success\"}";
                int ret = send(sockfd, buf, strlen(buf), 0);
        }
        else if(!strcmp(json_object_get_string(json), "circle")){
                printf("received [circle]\n");
                const char *buf = "{\"cmd\": \"reply\", \"result\": \"success\"}";
                int ret = send(sockfd, buf, strlen(buf), 0);
        }
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