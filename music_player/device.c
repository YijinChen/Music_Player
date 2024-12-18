//control the hardware relative operation
#include <fcntl.h>
#include <sys/ioctl.h>
#include "main.h"
//#include "select.h"
//#include "player.h"
#include "socket.h"
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
int flash_flag = 0;

void led_on(){
    int status = 1;
    write(g_ledfd, &status, 1);
}

void led_off(){
    int status = 0;
    write(g_ledfd, &status, 1);
}

void flash_led_on() {
    flash_flag = 1;  // Set the flash flag to true
    int interval = 200; // 200 milliseconds on and off

    while (flash_flag) {
        led_on();
        usleep(interval * 1000);  // Wait for 'interval' milliseconds
        led_off();
        usleep(interval * 1000);  // Wait for 'interval' milliseconds
    }
}

void flash_led_off() {
    flash_flag = 0;  // Stop flashing
    led_off();  // Ensure the LED is turned off
}

int InitDriver(){
    //Open the button device file
    g_buttonfd = open("/dev/100ask_gpio_key", O_RDWR);
    if (g_buttonfd == -1){
        return FAILURE;
    }

	// Set the button file descriptor to non-blocking mode
    int flags = fcntl(g_buttonfd, F_GETFL, 0);
    fcntl(g_buttonfd, F_SETFL, flags | O_NONBLOCK);

    //Open the led device file
    g_ledfd = open("/dev/myled", O_WRONLY);
    if (g_ledfd == -1){
        return FAILURE;
    }

    //turn off led
    int status = 0;
    led_off();

    if(g_ledfd > g_maxfd){
        g_maxfd = g_ledfd;
    }

    return SUCCESS;
}

int get_key_id(){
	int fd;
	int val;
	int gpio_num, gpio_key;

    // while (1){
        /* 3. Read from the file */
        read(g_buttonfd, &val, sizeof(val));  // Read the full "key" (which includes both GPIO number and state)

        /* 4. Extract GPIO number and state */
        gpio_num = val >> 8;  // Extract the upper byte (GPIO number)
        gpio_key = val & 0xFF;  // Extract the lower byte (GPIO key)
		//printf("GPIO %d, State %d\n", gpio_num, gpio_state);
		int key = 0;

		if (gpio_key == 1) //quick press
		{
			switch (gpio_num)
			{
			case 129:
				key = 1;
				break;
			case 110:
				key = 2;
				break;
			case 115:
				key = 3;
				break;
			case 116:
				key = 4;
				break;
			case 117:
				key = 5;
				break;
			case 118:
				key = 6;
				break;
			default:
				break;
			}
		}
        if (gpio_key == 2) //long press
		{
			switch (gpio_num)
			{
			case 115:
				key = 7;
				break;
			case 116:
				key = 8;
				break;
			case 117:
				key = 9;
				break;
			case 118:
				key = 10;
				break;
			default:
				break;
			}
		}
		/* Only print the pressed button when it's a valid button press */
        if (key != 0)
        {
            //printf("Pressed button: %d\n", key);
            return key;
        }
    //}
}