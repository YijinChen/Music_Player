#ifndef SELECT_H
#define SELECT_H
#include <sys/select.h>
#include <sys/types.h>

extern int g_buttonfd;
extern int g_sockfd;
extern int g_ledfd;
extern int g_mixerfd; // control the voice volumn
extern fd_set readfd;
extern int g_maxfd;

void m_select();

#endif