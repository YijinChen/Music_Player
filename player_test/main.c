#include <stdio.h>
#include "main.h"
#include "device.h"
#include "socket.h"
#include "link.h"
#include <stdlib.h> // for exit()
#include "player.h"
#include "select.h"
#include <json-c/json.h>

//arm-unknown-linux-gnueabihf-gcc *.c -o main -ljson-c -lpthread -lmpg123 -lasound


int main(){
    int ret;
    ret = InitDriver(); // open device files
    if (ret == FAILURE){
        printf("Fail to initialize device files\n");
        exit(1);
    }
    else{
        printf("Succeed to initialize device files\n");
    }

    ret = InitSocket(); //initialize the network connection
    if(ret == FAILURE){
        printf("fail to initialize network connection");
        // if fail to connect, turn on 2 leds 
        // led_on(0);
        // led_on(1);
    }
    else{
        printf("Succeed to initialize network connection\n");
    }

    //initialize music list
    ret = InitLink();
    if(ret == FAILURE){
        printf("fail to initialize music list\n");
        exit(1);
    }
    else{
        printf("Succeed to initialize music list\n");
    }

    //Initialize shared memory
    ret = InitShm();
    if(ret == FAILURE){
        printf("fail to initialize shared memory\n");
        exit(1);
    }
    else{
        printf("Succeed to initialize shared memory\n");
    }

    GetMusic();
    m_select();

    return 0;
}