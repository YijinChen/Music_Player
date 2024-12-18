#include <stdio.h>
#include <sys/select.h>
#include "player.h"
#include "device.h"
#include <json-c/json.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>  // For fd_set and other types
#include "socket.h"
#include <errno.h>
#include <unistd.h> // for sleep()
#include <pthread.h>
#include "main.h"
// #include <sys/time.h>   // For struct timeval
// #include <unistd.h>     // For close and other POSIX functions

fd_set tmpfd;

void parse_message(const char *m, char *c, size_t len){
    struct json_object *obj = json_tokener_parse(m);
    struct json_object *json;

    if (json_object_object_get_ex(obj, "cmd", &json)) {
        const char *cmd_str = json_object_get_string(json);
        strncpy(c, cmd_str, len - 1); // Use strncpy to avoid buffer overflow
        c[len - 1] = '\0'; // Ensure null-termination
    }

    json_object_put(obj); // Free the JSON object
}

void show(){
    printf("start                ---- quick press [key1]\n");
    printf("stop                 ---- quick press [key2]\n");
    printf("suspend              ---- quick press [key3]\n");
    printf("resume               ---- quick press [key4]\n");
    printf("volume up            ---- quick press [key5]\n");
    printf("volume down          ---- quick press [key6]\n");
    printf("previous one         ---- long  press [key3]\n");
    printf("next one             ---- long  press [key4]\n");
    printf("change playing mode: sequence/random/circle\n");
    printf("                     ---- long  press [key5]\n");
    printf("reconnect to server  ---- long  press [key6]\n");
}


void InitSelect()
{
	FD_ZERO(&readfd);
	FD_ZERO(&tmpfd);
	FD_SET(g_buttonfd, &readfd);
}

void m_select(){
    show();
    int ret;
    char message[3072] = {0};
    //FD_SET(0, &readfd);    // for code testing, add standard input into readfd

    while(1){
        tmpfd = readfd;
        ret = select(g_maxfd + 1, &tmpfd, NULL, NULL, NULL);
        if (ret == -1 && errno != EINTR){ //avoid "select: Interrupted system call"
            perror("select");
            continue;
        }
        
        if (connect_flag == 1 && FD_ISSET(g_sockfd, &tmpfd)){
            //if data is sent by tcp
            memset(message, 0, sizeof(message));
            char cmd[128] = {0};
            ret = recv(g_sockfd, message, sizeof(message), 0);
            if(ret > 0){
                parse_message(message, cmd, sizeof(cmd));
                //printf("get message from socket: %s\n", cmd);
            }
            else if(ret == -1){
                perror("recv");
                continue;
            }
            else{   //ret == 0
                printf("Connection stoped by server\n");
                connect_flag = 0;
                alarm(0); //Stop sending "keep alive", which began in connect_cb
                continue;
            };

            if(!strcmp(cmd, "start")){
                if(g_suspend_flag == 0){
                    socket_start_play();
                }
                else{
                    socket_resume_play();
                }
            }
            else if(!strcmp(cmd, "stop")){
                socket_stop_play();
            }
            else if(!strcmp(cmd, "suspend")){
                socket_suspend_play();
            }
            else if(!strcmp(cmd, "previous")){
                socket_previous_play();
            }
            else if(!strcmp(cmd, "next")){
                socket_next_play();
            }
            else if(!strcmp(cmd, "volume_up")){
                socket_volume_up_play();
            }
            else if(!strcmp(cmd, "volume_down")){
                socket_volume_down_play();
            }
            else if(!strcmp(cmd, "sequence")){
                socket_mode_play(SEQUENCEMODE);
            }
            else if(!strcmp(cmd, "random")){
                socket_mode_play(RANDOMMODE);
            }
            else if(!strcmp(cmd, "circle")){
                socket_mode_play(CIRCLEMODE);
            }
            else if(!strcmp(cmd, "get")){
                socket_get_status();
            }
            else if(!strcmp(cmd, "music")){
                socket_get_music();
            }
        }
        else if (FD_ISSET(g_buttonfd, &tmpfd)){
            //if data is sent by button
            int id = get_key_id();
            char name[64] = {0};
            pthread_t tid;
            int ret;
            //printf("get key id: %d\n", id);
            switch(id){
                case 1:
                    start_play(name);
                    break;
                case 2: 
                    stop_play();
                    break;
                case 3: 
                    suspend_play();
                    break;
                case 4: 
                    resume_play();
                    break;
                case 5:
                    volume_up();
                    break;
                case 6:
                    volume_down();
                    break;
                case 7:
                    previous_play(name);
                    break;
                case 8: 
                    next_play(name);
                    break;
                case 9: 
                    player_change_mode();
                    break;
                case 10: //reconnect network
                    //creat a thread and request the connection to server
                    ret = pthread_create(&tid, NULL, connect_cb, NULL);
                    if (ret != 0){
                        printf("Failed to create connect_cb pthread\n");
                    }
                    break;
            }
        }
        // else if(FD_ISSET(0, &tmpfd)){
        //     //use for testing on PC, data is sent by standard input   
        //     char func;
        //     scanf("%c", &func);

        //     switch(func){
        //         case '1':  //start play
        //             start_play();
        //             break;
        //         case '2': //stop play
        //             stop_play();
        //             break;
        //         case '3': //suspend
        //             suspend_play();
        //             break;
        //         case '4': //continue
        //             continue_play();
        //             break;
        //         case '5': //play last music
        //             prior_play();
        //             break;
        //         case '6':
        //             next_play();
        //             break;
        //         case '7':
        //             voice_up();
        //             break;
        //         case '8':
        //             voice_down();
        //             break;
        //         case '9':
        //             set_mode(SEQUENCEMODE);
        //             break;
        //         case 'a':
        //             set_mode(RANDOMMODE);
        //             break;
        //         case 'b':
        //             set_mode(CIRCLEMODE);
        //             break;  
        //     }
        // }
    }
}