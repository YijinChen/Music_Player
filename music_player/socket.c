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
#include "player.h"
#include <sys/select.h>
#include <errno.h>
#include "link.h"

//every 5 seconds, send "alive" to server
void send_server(int sig){
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("info"));
    json_object_object_add(json, "status", json_object_new_string("alive"));
    json_object_object_add(json, "deviceid", json_object_new_string("001"));
    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
    alarm(5);
}

void my_sleep(int seconds) {
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = 0;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        // Loop until sleep completes successfully
        continue;
    }
}

void *connect_cb(void *arg){
    int count = 10;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while(count--){
        //when sending connection request to server, let the first led shine
        //led_on(0);
        printf("try to connect, count = %d\n", count);
        int ret = connect(g_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret == -1){
            printf("fail to connect\n");
            printf("Retrying connection in 5 seconds...\n");
            my_sleep(3); // If fail to connect, sleep 5 seconds
            printf("finish sleep\n");
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

        //sucessfully connect to server, add fd to set
        FD_SET(g_sockfd, &readfd);
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
        printf("In InitSocket: fail to creat pthread\n");
        return FAILURE;
    }

    if(g_sockfd > g_maxfd){
        g_maxfd = g_sockfd;
    }

    return SUCCESS;
}

void socket_start_play(){
    start_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_stop_play(){
    stop_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_suspend_play(){
    suspend_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_continue_play(){
    continue_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}


void socket_prior_play(){
    prior_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_next_play(){
    next_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_volume_up_play(){
    voice_up();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_volume_down_play(){
    voice_down();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_mode_play(int mode){
    set_mode(mode);

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

// void socket_start_play(){
//     start_play();

//     struct json_object *json = json_object_new_object();
//     json_object_object_add(json, "cmd", json_object_new_string("reply"));
//     json_object_object_add(json, "result", json_object_new_string("success"));

//     const char *buf = json_object_to_json_string(json);
//     int ret = send(g_sockfd, buf, strlen(buf), 0);
//     if(ret == -1){
//         perror("send");
//     }
// }

void socket_get_status(){
    //play status, current music, volume
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply_status"));
    if(g_start_flag == 1 && g_suspend_flag == 0){
        json_object_object_add(json, "status", json_object_new_string("start"));
    }
    else if(g_start_flag == 1 && g_suspend_flag == 1){
        json_object_object_add(json, "status", json_object_new_string("suspend"));
    }
    else if(g_start_flag == 0){
        json_object_object_add(json, "status", json_object_new_string("stop"));
    }
    json_object_object_add(json, "voice", json_object_new_int(iLeft));
    
    shm s;
    // Ensure g_addr is not NULL
    if (g_addr == NULL) {
        fprintf(stderr, "Error: g_addr is NULL\n");
        return;
    }
    memcpy(&s, g_addr, sizeof(s));
    // Ensure s.cur_name is properly null-terminated
    s.cur_name[sizeof(s.cur_name) - 1] = '\0';
    json_object_object_add(json, "music", json_object_new_string(s.cur_name));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_get_music(){
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply_music"));

    struct json_object *array = json_object_new_array();
    Node *p = head->next;
    while(p != head){
        json_object_array_add(array, json_object_new_string(p->music_name));//put music name into array
        p = p->next;
    }
    json_object_object_add(json, "music", array);

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}