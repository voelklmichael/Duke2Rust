// Linux stand-in for Watcom's <bios.h>.
#ifndef __COMPAT_BIOS_H
#define __COMPAT_BIOS_H
#include "compat.h"
// Ticks since midnight at 18.2 Hz.
unsigned _bios_timeofday(unsigned service, long *timeval);
// service 1: returns non-zero if a key is waiting.
unsigned _bios_keybrd(unsigned service);
#endif
