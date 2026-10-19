// Linux stand-in for Watcom's <dos.h>.
#ifndef __COMPAT_DOS_H
#define __COMPAT_DOS_H

#include "compat.h"
#include "i86.h"

#define _A_NORMAL 0x00
#define _A_RDONLY 0x01
#define _A_HIDDEN 0x02
#define _A_SYSTEM 0x04
#define _A_VOLID  0x08
#define _A_SUBDIR 0x10
#define _A_ARCH   0x20

struct find_t
{
	char reserved[21];
	char attrib;
	unsigned short wr_time;
	unsigned short wr_date;
	unsigned long size;
	char name[13];
	void *compat_state;   // PORT: directory iteration state
};

unsigned _dos_findfirst(const char *pattern, unsigned attrib, struct find_t *f);
unsigned _dos_findnext(struct find_t *f);
unsigned _dos_findclose(struct find_t *f);

void _disable(void);
void _enable(void);
void delay(unsigned ms);

#endif
