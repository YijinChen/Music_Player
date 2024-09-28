//control the hardware relative operation
#include <fcntl.h>
#include <sys/ioctl.h>
#include "main.h"
//#include "select.h"
//#include "player.h"
#include <poll.h>  //for "struct pollfd"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int g_buttonfd = 3;
int g_ledfd;

void led_on(int which){
    ioctl(g_ledfd, 1, which);
}

void led_off(int which){
    ioctl(g_ledfd, 0, which);
}

int InitDriver(){
    //Open the button device file
    g_buttonfd = open("/dev/100ask_gpio_key", O_RDWR | O_NONBLOCK);
    if (g_buttonfd == -1){
        return FAILURE;
    }

    //Open the led device file
    g_ledfd = open("/dev/myled", O_WRONLY);
    if (g_ledfd == -1){
        return FAILURE;
    }

    //turn off led
    int status = 0;
    write(g_ledfd, &status, 1);

    
    //Open the mixer device file
    // g_mixerfd = open("/dev/mixer", O_WRONLY);
    // if (g_mixerfd == -1){
    //     return FAILURE;
    // }

    // if(g_ledfd > g_maxfd){
    //     g_maxfd = g_ledfd;
    // }

    return SUCCESS;
}

int get_key_id(){
    int val;
	struct pollfd fds[1];
	int timeout_ms = 5000;
	int ret;
	int	flags;
	int gpio_num, gpio_state;
    int key;

    flags = fcntl(g_buttonfd, F_GETFL);
	fcntl(g_buttonfd, F_SETFL, flags & ~O_NONBLOCK);
	

    /* 3. Read from the file */
    read(g_buttonfd, &val, sizeof(val));  // Read the full "key" (which includes both GPIO number and state)

    /* 4. Extract GPIO number and state */
    gpio_num = val >> 8;  // Extract the upper byte (GPIO number)
    gpio_state = val & 0xFF;  // Extract the lower byte (GPIO state)
    if (gpio_state == 0)
    {
        switch (gpio_num)
        {
        case 110:
            key = 1;
            break;
        case 115:
            key = 2;
            break;
        case 119:
            key = 3;
            break;
        case 120:
            key = 4;
            break;
        case 121:
            key = 5;
            break;
        case 122:
            key = 6;
            break;
        default:
            break;
        }
    }
    
    /* 5. Print the result */
    //printf("GPIO %d, State %d\n", gpio_num, gpio_state);
    printf("Pressed button: %d\n", key);
}