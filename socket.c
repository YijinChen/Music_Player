//network relative code
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //for inet_addr
#include <netinet/in.h>     // For struct sockaddr_in
#include "main.h"
#include <pthread.h>
#include "socket.h"
#include <string.h>
#include <unistd.h> // For sleep()
#include "device.h"
#include "select.h"
#include <signal.h> //for signal()s
#include <json-c/json.h>

//every 5 seconds, send "alive" to server
void send_server(int sig){
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "status", json_object_new_string("alive"));
    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
    alarm(5);
}


void *connect_cb(void *arg){
    int count = 5;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    while(count--){
        //when sending connection request to server, let the first led shine
        //led_on(0);
        int ret = connect(g_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (ret == -1){
            //printf("fail to connect\n");
            sleep(5); // If fail to connect, sleep 5 seconds
            continue;
        }
        //when connection is successful, let all 4 leds shine
        led_on(0);
        led_on(1);
        led_on(2);
        led_on(3);

        //after 5 seconds, send SIGALRM to process
        alarm(5);
        signal(SIGALRM, send_server);

        break;
    }

    return NULL;
}

int InitSocket(){
    g_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    //printf("g_sockfd: %d\n", g_sockfd);
    if (g_sockfd == -1){
        return FAILURE;
    }

    //creat a thread and request the connection to server
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, connect_cb, NULL);
    //printf("ret: %d\n", ret);
    if (ret != 0){
        return FAILURE;
    }

    return SUCCESS;
}