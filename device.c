//control the hardware relative operation
#include <fcntl.h>
#include <sys/ioctl.h>
#include "main.h"

extern int g_buttonfd;
extern int g_ledfd;
extern int g_mixerfd;

int led_on(int which){
    ioctl(g_ledfd, 1, which);
}

int led_off(int which){
    ioctl(g_ledfd, 0, which);
}

int InitDriver(){
    //Open the button device file
    g_buttonfd = open("/dev/buttons",O_RDONLY);
    if (g_buttonfd == -1){
        return FAILURE;
    }

    //Open the led device file
    g_ledfd = open("/dev/leds", O_WRONLY);
    if (g_ledfd == -1){
        return FAILURE;
    }

    //turn off all 4 leds
    for(int i; i < 4; i++){
        ioctl(g_ledfd, 0, i);
    }


    //Open the mixer device file
    g_mixerfd = open("/dev/mixer", O_WRONLY);
    if (g_mixerfd == -1){
        return FAILURE;
    }

    return SUCCESS;
}