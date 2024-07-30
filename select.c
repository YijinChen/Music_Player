#include <stdio.h>
#include <sys/select.h>
#include "player.h"
// #include <sys/types.h>  // For fd_set and other types
// #include <sys/time.h>   // For struct timeval
// #include <unistd.h>     // For close and other POSIX functions

int g_buttonfd = 3;
int g_sockfd = 3;
int g_ledfd;
int g_mixerfd; // control the voice volumn

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
    fd_set readfd, tmpfd;
    //int maxfd = (g_buttonfd > g_sockfd) ? g_buttonfd : g_sockfd;
    int maxfd = 2;
    int ret;
    
    FD_ZERO(&readfd);
    FD_ZERO(&tmpfd);

    //FD_SET(g_buttonfd, &readfd);
    //FD_SET(g_sockfd, &readfd);
    FD_SET(0, &readfd);    // add standard input into readfd

    while(1){
        tmpfd = readfd;
        ret = select(maxfd + 1, &tmpfd, NULL, NULL, NULL);
        if (ret == -1){
            perror("select");
        }

        if (FD_ISSET(g_sockfd, &tmpfd)){
            //if data is sent by tcp

        }
        else if (FD_ISSET(g_buttonfd, &tmpfd)){
            //if data is sent by button

        }
        else if(FD_ISSET(0, &tmpfd)){
            //use for testing on PC, data is sent by standard input   
            char func;
            scanf("%c", &func);

            switch(func){
                case '1':  //start play
                    start_play();
                    break;
                case '2': //stop play
                    stop_play();
                    break;
                case '3': //suspend
                    suspend_play();
                    break;
                case '4': //continue
                    continue_play();
                    break;
                case '5': //play last music
                    prior_play();
                    break;
                case '6':
                    next_play();
                    break;
                // case '7':
                // voice_up();
                // break;
                // case '8':
                // voice_down();
                // break;
                case '9':
                    set_mode(SEQUENCEMODE);
                    break;
                case 'a':
                    set_mode(RANDOMMODE);
                    break;
                case 'b':
                    set_mode(CIRCLEMODE);
                    break;  
            }
        }

    }


}