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
//#include "select.h"
#include <signal.h> //for signal()s
#include <json-c/json.h>
#include "player.h"
#include <sys/select.h>
#include <errno.h>
#include "link.h"
#include <fcntl.h>

int g_sockfd = 0;
fd_set readfd;
int g_maxfd = 0;
int connect_flag = 0;

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
    alarm(TIMEOUT);
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

void *flash_led_thread(void *arg) {
    flash_led_on();  // This will run in a separate thread
    return NULL;
}

void *connect_cb(void *arg){
    if(connect_flag == 1){
        printf("Sucessfully connected to server\n");
        return NULL;
    }
    int count = 5;
    pthread_t led_thread;  // Declare a thread for LED shining

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Start LED flash in a separate thread
    if (pthread_create(&led_thread, NULL, flash_led_thread, NULL) != 0) {
        perror("Failed to create LED thread");
        return NULL;
    }

    while (count--) {
        // The LED is already shining in the background
        printf("Connecting to server ...\n");
        int ret = connect(g_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret == -1) {
            my_sleep(3);  // If connection fails, sleep for 3 seconds
            continue;
        }

        printf("Sucessfully connect to server\n");
        FD_SET(g_sockfd, &readfd); // add g_sockfd into readfd
        // When connection is successful, shop led flashing, keep led on
        flash_led_off();  // Turn off LED falshing
        // Optionally wait for the thread to finish (ensure the LED stops flashing)
        pthread_join(led_thread, NULL);
        led_on();

        // After 5 seconds, send SIGALRM to process
        alarm(TIMEOUT);
        signal(SIGALRM, send_server);

        connect_flag = 1;
        break;
    }

    if (connect_flag == 0) {
        printf("Failed to connect to server, please try again (long press key 6)\n");
        flash_led_off();  // Make sure to stop shining even if connection fails
        pthread_join(led_thread, NULL);  // Ensure the LED thread has stopped
    }

    return NULL;
}

int InitSocket(){
    g_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (g_sockfd == -1){
        return FAILURE;
    }
    fcntl(g_sockfd, F_SETFL, O_NONBLOCK);

    //creat a thread and request the connection to server
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, connect_cb, NULL);
    //printf("ret: %d\n", ret);
    if (ret != 0){
        printf("In InitSocket: fail to create pthread\n");
        return FAILURE;
    }

    if(g_sockfd > g_maxfd){
        g_maxfd = g_sockfd;
    }

    return SUCCESS;
}

void socket_start_play(){
    char name[64] = {0};
    start_play(name);
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("start_success"));
    long volume;
    volume = get_volume();
    json_object_object_add(json, "volume", json_object_new_int64(volume));
    json_object_object_add(json, "music", json_object_new_string(name));

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
    json_object_object_add(json, "result", json_object_new_string("stop_success"));

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
    json_object_object_add(json, "result", json_object_new_string("suspend_success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_resume_play(){
    resume_play();

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("resume_success"));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_previous_play(){
    char name[64] = {0};
    previous_play(name);

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("previous_success"));
    json_object_object_add(json, "music", json_object_new_string(name));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_next_play(){
    char name[64] = {0};
    next_play(name);

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("next_success"));
    json_object_object_add(json, "music", json_object_new_string(name));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_volume_up_play(){
    volume_up(); //the result will show on QT app as increase 5%

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("volume_success"));
    long volume;
    volume = get_volume();
    json_object_object_add(json, "volume", json_object_new_int64(volume));

    const char *buf = json_object_to_json_string(json);
    int ret = send(g_sockfd, buf, strlen(buf), 0);
    if(ret == -1){
        perror("send");
    }
}

void socket_volume_down_play(){
    volume_down(); //the result will show on QT app as decrease 5%
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "cmd", json_object_new_string("reply"));
    json_object_object_add(json, "result", json_object_new_string("volume_success"));
    long volume;
    volume = get_volume();
    json_object_object_add(json, "volume", json_object_new_int64(volume));

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

void socket_get_status(){
    printf("Entered socket_get_status...\n");
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
    long volume;
    volume = get_volume();
    json_object_object_add(json, "volume", json_object_new_int64(volume));
    
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
    if(connect_flag == 1){
        int ret = send(g_sockfd, buf, strlen(buf), 0);
        if(ret == -1){
            perror("send");
        }
        printf("send status to server\n");
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