// Linux stand-in for Watcom's <conio.h>.
#ifndef __COMPAT_CONIO_H
#define __COMPAT_CONIO_H
#include "compat.h"
int getch(void);
int kbhit(void);
int inp(unsigned port);
int outp(unsigned port, int value);
#endif
