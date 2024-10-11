#ifndef DEVICE_H
#define DEVICE_H

int InitDriver();
void led_on();
void led_off();
int get_key_id();

extern int g_buttonfd;
extern int g_ledfd;

#endif