#ifndef DEVICE_H
#define DEVICE_H

int InitDriver();
void led_on();
void led_off();
int get_key_id();
void stop_shining(int signum);
void shine_led_on();
void shine_led_off();

extern int g_buttonfd;
extern int g_ledfd;

#endif