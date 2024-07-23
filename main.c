#include <stdio.h>
#include "main.h"
#include "device.h"
#include "socket.h"
#include "link.h"
#include <stdlib.h> // for exit()
#include "player.h"


int g_buttonfd = 3;
int g_ledfd;
int g_mixerfd; // control the voice volumn
int g_sockfd = 3;
struct Node *head; //head for music list

int main(){
    int ret;
    /*ret = InitDriver(); // open device files
    if (ret == FAILURE){
        printf("Fail to initialize device files");
        exit(1);
    }*/

    ret = InitSocket(); //initialize the network connection
    if(ret == FAILURE){
        printf("fail to initialize network connection");
        // if fail to connect, turn on 2 leds 
        led_on(0);
        led_on(1);
    }

    //initialize music list
    ret = InitLink();
    if(ret == FAILURE){
        printf("fail to initialize music list");
        exit(1);
    }

    //Initialize shared memory
    ret = InitShm();
    if(ret == FAILURE){
        print("fail to initialize shared memory\n");
        exit(1);
    }

    GetMusic();
    m_select();


    return 0;
}