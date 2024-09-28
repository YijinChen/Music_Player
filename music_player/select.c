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
// #include <sys/time.h>   // For struct timeval
// #include <unistd.h>     // For close and other POSIX functions


int g_sockfd = 3;
//int g_mixerfd; // control the voice volumn
fd_set readfd;
int g_maxfd = 0;


// void parse_message(const char *m, char *c){
//     struct json_object *obj = json_tokener_parse(m);
//     struct json_object *json;

//     json_object_object_get_ex(obj, "cmd", &json);
//     strcpy(c, json_object_get_string(json));
// }

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
    printf("1. start\n");
    printf("2. stop\n");
    printf("3. suspend\n");
    printf("4. continue\n");
    printf("5. last\n");
    printf("6. next\n");
    printf("7. increase volume\n");
    printf("8. decrease volume\n");
    printf("9. sequence mode\n");
    printf("a. random mode\n");
    printf("b. circle mode\n");
}


void m_select(){
    show();
    fd_set tmpfd;
    //int maxfd = (g_buttonfd > g_sockfd) ? g_buttonfd : g_sockfd;
    //int maxfd = 3;
    int ret;
    char message[1024] = {0};
    
    FD_ZERO(&readfd);
    FD_ZERO(&tmpfd);

    FD_SET(g_buttonfd, &readfd);
    FD_SET(g_sockfd, &readfd);
    //FD_SET(0, &readfd);    // add standard input into readfd

    while(1){
        tmpfd = readfd;
        ret = select(g_maxfd + 1, &tmpfd, NULL, NULL, NULL);
        if (ret == -1 && errno != EINTR){ //avoid "select: Interrupted system call"
            perror("select");
        }

        if (FD_ISSET(g_sockfd, &tmpfd)){
            //if data is sent by tcp
            memset(message, 0, sizeof(message));
            ret = recv(g_sockfd, message, sizeof(message), 0);
            if(ret == -1){
                perror("recv");
            }

            char cmd[128] = {0};
            //printf("In select.c: ready to parse_message\n");
            parse_message(message, cmd, sizeof(cmd));
            //printf("In select.c: finish parse_message\n");

            if(!strcmp(cmd, "start")){
                socket_start_play();
            }
            else if(!strcmp(cmd, "stop")){
                socket_stop_play();
            }
            else if(!strcmp(cmd, "suspend")){
                socket_suspend_play();
            }
            else if(!strcmp(cmd, "continue")){
                socket_continue_play();
            }
            else if(!strcmp(cmd, "prior")){
                socket_prior_play();
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
                printf("received get\n");
                socket_get_status();
            }
            else if(!strcmp(cmd, "music")){
                socket_get_music();
            }
        }
        else if (FD_ISSET(g_buttonfd, &tmpfd)){
            //if data is sent by button
            int id = get_key_id();
            printf("get key id: %d\n", id);
            switch(id){
                case '1':  //start play
                    start_play();
                    break;
                case '2': //stop play
                    stop_play();
                    break;
                case '3': //suspend
                    prior_play();
                    break;
                case '4': //continue
                    next_play();
                    break;
                case '5': //play last music
                    voice_up();
                    break;
                case '6':
                    voice_down();
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