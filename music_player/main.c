#include <stdio.h>
#include "main.h"
#include "device.h"
#include "socket.h"
#include "link.h"
#include <stdlib.h> // for exit()
#include "player.h"
#include "select.h"
#include <json-c/json.h>

//gcc *.c -o main -ljson-c -lpthread


int main(){
    int ret;
    // ret = InitDriver(); // open device files
    // if (ret == FAILURE){
    //     printf("Fail to initialize device files");
    //     exit(1);
    // }

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
        printf("fail to initialize music list\n");
        exit(1);
    }

    //Initialize shared memory
    ret = InitShm();
    if(ret == FAILURE){
        printf("fail to initialize shared memory\n");
        exit(1);
    }

    GetMusic();
    printf("In main.c: ready to select\n");
    m_select();
    printf("In main.c: finish to select\n");


    return 0;
}