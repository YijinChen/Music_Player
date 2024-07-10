#include <stdio.h>
#include "main.h"
#include "device.h"
#include <stdlib.h> // for exit()


int g_buttonfd;
int g_ledfd;
int g_mixerfd; // control the voice volumn
int g_sockfd;

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


    return 0;
}