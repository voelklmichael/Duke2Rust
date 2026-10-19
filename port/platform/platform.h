// Platform layer: everything that talked to DOS hardware goes through here.
#ifndef __PLATFORM_H
#define __PLATFORM_H

// Video: an 8-bit paletted framebuffer.
int  platform_setvideomode(long xdim, long ydim);
void platform_closevideo(void);
void platform_present(const unsigned char *frame, long xdim, long ydim, long pitch);
// Palette entries are 4 bytes: blue, green, red, unused (6-bit VGA values).
void platform_setpalette(long start, long num, const unsigned char *pal);
void platform_getpalette(long start, long num, unsigned char *pal);
// BIOS video mode (0x3 text, 0x13 VGA 320x200): only used at start/exit.
void platform_setvmode(long mode);
int  platform_mousepresent(void);
void platform_readmousexy(short *dx, short *dy);
int  platform_readmousebuttons(void);
void platform_outp(long port, long val);
long platform_inp(long port);
unsigned long platform_ticks_us(void);
void platform_sleep_ms(unsigned ms);
int  platform_kbhit(void);
int  platform_getch(void);

#endif
