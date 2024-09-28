#ifndef DEVICE_H
#define DEVICE_H

int InitDriver();
void led_on(int which);
void led_off(int which);
int get_key_id();

extern int g_buttonfd;
extern int g_ledfd;

#endif