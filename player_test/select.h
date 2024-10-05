#ifndef SELECT_H
#define SELECT_H
#include <sys/select.h>
#include <sys/types.h>
#include "device.h"

extern fd_set tmpfd;
void InitSelect();
void m_select();

#endif