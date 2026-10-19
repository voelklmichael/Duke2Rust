// See pragmas.h.
#include <string.h>
#include "pragmas.h"
#include "platform.h"

typedef long long i64;
typedef unsigned long long u64;

long dmval;

// Low 32 bits of a 64-bit value (what "shrd eax, edx, n" leaves in eax).
#define LO32(x) ((long)(unsigned long)(u64)(x))

long sqr(long a) { return (long)((unsigned long)a * (unsigned long)a); }
long scale(long a, long b, long c) { return LO32(((i64)a * b) / c); }
long mulscale(long a, long b, long s) { return LO32(((i64)a * b) >> (s & 31)); }
long mulscale1(long a, long b) { return LO32(((i64)a * b) >> 1); }
long mulscale2(long a, long b) { return LO32(((i64)a * b) >> 2); }
long mulscale3(long a, long b) { return LO32(((i64)a * b) >> 3); }
long mulscale4(long a, long b) { return LO32(((i64)a * b) >> 4); }
long mulscale5(long a, long b) { return LO32(((i64)a * b) >> 5); }
long mulscale6(long a, long b) { return LO32(((i64)a * b) >> 6); }
long mulscale7(long a, long b) { return LO32(((i64)a * b) >> 7); }
long mulscale8(long a, long b) { return LO32(((i64)a * b) >> 8); }
long mulscale9(long a, long b) { return LO32(((i64)a * b) >> 9); }
long mulscale10(long a, long b) { return LO32(((i64)a * b) >> 10); }
long mulscale11(long a, long b) { return LO32(((i64)a * b) >> 11); }
long mulscale12(long a, long b) { return LO32(((i64)a * b) >> 12); }
long mulscale13(long a, long b) { return LO32(((i64)a * b) >> 13); }
long mulscale14(long a, long b) { return LO32(((i64)a * b) >> 14); }
long mulscale15(long a, long b) { return LO32(((i64)a * b) >> 15); }
long mulscale16(long a, long b) { return LO32(((i64)a * b) >> 16); }
long mulscale17(long a, long b) { return LO32(((i64)a * b) >> 17); }
long mulscale18(long a, long b) { return LO32(((i64)a * b) >> 18); }
long mulscale19(long a, long b) { return LO32(((i64)a * b) >> 19); }
long mulscale20(long a, long b) { return LO32(((i64)a * b) >> 20); }
long mulscale21(long a, long b) { return LO32(((i64)a * b) >> 21); }
long mulscale22(long a, long b) { return LO32(((i64)a * b) >> 22); }
long mulscale23(long a, long b) { return LO32(((i64)a * b) >> 23); }
long mulscale24(long a, long b) { return LO32(((i64)a * b) >> 24); }
long mulscale25(long a, long b) { return LO32(((i64)a * b) >> 25); }
long mulscale26(long a, long b) { return LO32(((i64)a * b) >> 26); }
long mulscale27(long a, long b) { return LO32(((i64)a * b) >> 27); }
long mulscale28(long a, long b) { return LO32(((i64)a * b) >> 28); }
long mulscale29(long a, long b) { return LO32(((i64)a * b) >> 29); }
long mulscale30(long a, long b) { return LO32(((i64)a * b) >> 30); }
long mulscale31(long a, long b) { return LO32(((i64)a * b) >> 31); }
long mulscale32(long a, long b) { return LO32(((i64)a * b) >> 32); }
long dmulscale(long a, long b, long c, long d, long s) { return LO32(((i64)a * b + (i64)c * d) >> (s & 31)); }
long dmulscale1(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 1); }
long dmulscale2(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 2); }
long dmulscale3(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 3); }
long dmulscale4(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 4); }
long dmulscale5(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 5); }
long dmulscale6(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 6); }
long dmulscale7(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 7); }
long dmulscale8(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 8); }
long dmulscale9(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 9); }
long dmulscale10(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 10); }
long dmulscale11(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 11); }
long dmulscale12(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 12); }
long dmulscale13(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 13); }
long dmulscale14(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 14); }
long dmulscale15(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 15); }
long dmulscale16(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 16); }
long dmulscale17(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 17); }
long dmulscale18(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 18); }
long dmulscale19(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 19); }
long dmulscale20(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 20); }
long dmulscale21(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 21); }
long dmulscale22(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 22); }
long dmulscale23(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 23); }
long dmulscale24(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 24); }
long dmulscale25(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 25); }
long dmulscale26(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 26); }
long dmulscale27(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 27); }
long dmulscale28(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 28); }
long dmulscale29(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 29); }
long dmulscale30(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 30); }
long dmulscale31(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 31); }
long dmulscale32(long a, long b, long c, long d) { return LO32(((i64)a * b + (i64)c * d) >> 32); }
long tmulscale1(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 1); }
long tmulscale2(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 2); }
long tmulscale3(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 3); }
long tmulscale4(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 4); }
long tmulscale5(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 5); }
long tmulscale6(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 6); }
long tmulscale7(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 7); }
long tmulscale8(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 8); }
long tmulscale9(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 9); }
long tmulscale10(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 10); }
long tmulscale11(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 11); }
long tmulscale12(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 12); }
long tmulscale13(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 13); }
long tmulscale14(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 14); }
long tmulscale15(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 15); }
long tmulscale16(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 16); }
long tmulscale17(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 17); }
long tmulscale18(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 18); }
long tmulscale19(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 19); }
long tmulscale20(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 20); }
long tmulscale21(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 21); }
long tmulscale22(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 22); }
long tmulscale23(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 23); }
long tmulscale24(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 24); }
long tmulscale25(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 25); }
long tmulscale26(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 26); }
long tmulscale27(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 27); }
long tmulscale28(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 28); }
long tmulscale29(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 29); }
long tmulscale30(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 30); }
long tmulscale31(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 31); }
long tmulscale32(long a, long b, long c, long d, long e, long f) { return LO32(((i64)a * b + (i64)c * d + (i64)e * f) >> 32); }
long boundmulscale(long a, long b, long s)
{
	i64 p = (i64)a * b;
	i64 r = p >> (s & 31);
	// Saturate if the shifted product does not fit in 32 bits.
	if (r != (i64)(long)r) return (p < 0) ? (long)0x80000000 : 0x7fffffff;
	return (long)r;
}
long divscale(long a, long b, long s) { return LO32((i64)((u64)(i64)a << (s & 31)) / b); }
long divscale1(long a, long b) { return LO32((i64)((u64)(i64)a << 1) / b); }
long divscale2(long a, long b) { return LO32((i64)((u64)(i64)a << 2) / b); }
long divscale3(long a, long b) { return LO32((i64)((u64)(i64)a << 3) / b); }
long divscale4(long a, long b) { return LO32((i64)((u64)(i64)a << 4) / b); }
long divscale5(long a, long b) { return LO32((i64)((u64)(i64)a << 5) / b); }
long divscale6(long a, long b) { return LO32((i64)((u64)(i64)a << 6) / b); }
long divscale7(long a, long b) { return LO32((i64)((u64)(i64)a << 7) / b); }
long divscale8(long a, long b) { return LO32((i64)((u64)(i64)a << 8) / b); }
long divscale9(long a, long b) { return LO32((i64)((u64)(i64)a << 9) / b); }
long divscale10(long a, long b) { return LO32((i64)((u64)(i64)a << 10) / b); }
long divscale11(long a, long b) { return LO32((i64)((u64)(i64)a << 11) / b); }
long divscale12(long a, long b) { return LO32((i64)((u64)(i64)a << 12) / b); }
long divscale13(long a, long b) { return LO32((i64)((u64)(i64)a << 13) / b); }
long divscale14(long a, long b) { return LO32((i64)((u64)(i64)a << 14) / b); }
long divscale15(long a, long b) { return LO32((i64)((u64)(i64)a << 15) / b); }
long divscale16(long a, long b) { return LO32((i64)((u64)(i64)a << 16) / b); }
long divscale17(long a, long b) { return LO32((i64)((u64)(i64)a << 17) / b); }
long divscale18(long a, long b) { return LO32((i64)((u64)(i64)a << 18) / b); }
long divscale19(long a, long b) { return LO32((i64)((u64)(i64)a << 19) / b); }
long divscale20(long a, long b) { return LO32((i64)((u64)(i64)a << 20) / b); }
long divscale21(long a, long b) { return LO32((i64)((u64)(i64)a << 21) / b); }
long divscale22(long a, long b) { return LO32((i64)((u64)(i64)a << 22) / b); }
long divscale23(long a, long b) { return LO32((i64)((u64)(i64)a << 23) / b); }
long divscale24(long a, long b) { return LO32((i64)((u64)(i64)a << 24) / b); }
long divscale25(long a, long b) { return LO32((i64)((u64)(i64)a << 25) / b); }
long divscale26(long a, long b) { return LO32((i64)((u64)(i64)a << 26) / b); }
long divscale27(long a, long b) { return LO32((i64)((u64)(i64)a << 27) / b); }
long divscale28(long a, long b) { return LO32((i64)((u64)(i64)a << 28) / b); }
long divscale29(long a, long b) { return LO32((i64)((u64)(i64)a << 29) / b); }
long divscale30(long a, long b) { return LO32((i64)((u64)(i64)a << 30) / b); }
long divscale31(long a, long b) { return LO32((i64)((u64)(i64)a << 31) / b); }
long divscale32(long a, long b) { return LO32((i64)((u64)(i64)a << 32) / b); }

long mul3(long a) { return (long)((unsigned long)a * 3); }
long mul5(long a) { return (long)((unsigned long)a * 5); }
long mul9(long a) { return (long)((unsigned long)a * 9); }

// Unsigned division; the other half of the result goes to dmval.
long divmod(long a, long b) { dmval = (long)((unsigned long)a % (unsigned long)b); return (long)((unsigned long)a / (unsigned long)b); }
long moddiv(long a, long b) { dmval = (long)((unsigned long)a / (unsigned long)b); return (long)((unsigned long)a % (unsigned long)b); }

long klabs(long a) { return (a < 0) ? (long)(0u - (unsigned long)a) : a; }
long ksgn(long a) { return (a > 0) - (a < 0); }
long umin(long a, long b) { return ((unsigned long)a < (unsigned long)b) ? a : b; }
long umax(long a, long b) { return ((unsigned long)a > (unsigned long)b) ? a : b; }
long kmin(long a, long b) { return (a < b) ? a : b; }
long kmax(long a, long b) { return (a > b) ? a : b; }


// The byte variants store the same byte/word/dword pattern as the original
// rep stos/movs sequences (the fill value is taken per store width).
void clearbuf_(long d, long c, long a)
{
	unsigned long *p = (unsigned long *)d;
	while (c-- > 0) *p++ = (unsigned long)a;
}

void clearbufbyte_(long d, long c, long a)
{
	unsigned char *p = (unsigned char *)d;
	unsigned short w = (unsigned short)a;
	if (c < 4)
	{
		if (c & 1) *p++ = (unsigned char)a;
		for (c >>= 1; c > 0; c--, p += 2) memcpy(p, &w, 2);
		return;
	}
	if ((unsigned long)p & 1) { *p++ = (unsigned char)a; c--; }
	if ((unsigned long)p & 2) { memcpy(p, &w, 2); p += 2; c -= 2; }
	{
		long n = c >> 2;
		for (; n > 0; n--, p += 4) memcpy(p, &a, 4);
	}
	if (c & 2) { memcpy(p, &w, 2); p += 2; }
	if (c & 1) *p = (unsigned char)a;
}

void copybuf_(long s, long d, long c)
{
	unsigned long *ps = (unsigned long *)s, *pd = (unsigned long *)d;
	while (c-- > 0) *pd++ = *ps++;
}

// Forward byte copy (overlapping regions behave like rep movs).
void copybufbyte_(long s, long d, long c)
{
	unsigned char *ps = (unsigned char *)s, *pd = (unsigned char *)d;
	while (c-- > 0) *pd++ = *ps++;
}

// Copies c bytes, reading backwards from s and writing forwards to d.
void copybufreverse_(long s, long d, long c)
{
	unsigned char *ps = (unsigned char *)s, *pd = (unsigned char *)d;
	while (c-- > 0) *pd++ = *ps--;
}

void qinterpolatedown16_(long bufptr, long num, long val, long add)
{
	long *p = (long *)bufptr;
	for (; num > 0; num--) { *p++ = val >> 16; val += add; }
}

void qinterpolatedown16short_(long bufptr, long num, long val, long add)
{
	short *p = (short *)bufptr;
	for (; num > 0; num--) { *p++ = (short)(val >> 16); val += add; }
}

void swapchar_(long a, long b) { unsigned char t = *(unsigned char *)a; *(unsigned char *)a = *(unsigned char *)b; *(unsigned char *)b = t; }
void swapshort_(long a, long b) { short t = *(short *)a; *(short *)a = *(short *)b; *(short *)b = t; }
void swaplong_(long a, long b) { long t = *(long *)a; *(long *)a = *(long *)b; *(long *)b = t; }
void swapbuf4_(long a, long b, long c) { for (; c > 0; c--, a += 4, b += 4) swaplong_(a, b); }
void swap64bit_(long a, long b) { swaplong_(a, b); swaplong_(a + 4, b + 4); }
// Same as swapchar(a,b); swapchar(a+1,b+xsiz);
void swapchar2_(long a, long b, long xsiz) { swapchar_(a, b); swapchar_(a + 1, b + xsiz); }

long readpixel_(long p) { return *(unsigned char *)p; }
void drawpixel_(long p, long c) { *(unsigned char *)p = (unsigned char)c; }
void drawpixels_(long p, long c) { unsigned short w = (unsigned short)c; memcpy((void *)p, &w, 2); }
void drawpixelses_(long p, long c) { memcpy((void *)p, &c, 4); }


void int5(void) { }
void setvmode(long mode) { platform_setvmode(mode); }
long setupmouse(void) { return platform_mousepresent() ? 0xffff : 0; }
void readmousexy_(long x, long y) { short dx, dy; platform_readmousexy(&dx, &dy); *(short *)x = dx; *(short *)y = dy; }
void readmousebstatus_(long b) { *(short *)b = (short)platform_readmousebuttons(); }
void koutp(long port, long val) { platform_outp(port, val); }
void koutpw(long port, long val) { platform_outp(port, val & 255); platform_outp(port + 1, (val >> 8) & 255); }
long kinp(long port) { return platform_inp(port); }
void limitrate(void) { }
long readtimer(void) { return 0; }

// 16-colour (EGA planar) 2D editor mode and stereo blits: unused by the game.
void setcolor16(long col) { (void)col; }
void vlin16first_(long p, long c) { (void)p; (void)c; }
void vlin16_(long p, long c) { (void)p; (void)c; }
void drawpixel16(long p) { (void)p; }
void fillscreen16_(long p, long col, long c) { (void)p; (void)col; (void)c; }
void redblueblit_(long a, long b, long c) { (void)a; (void)b; (void)c; }
void chainblit_(long s, long d, long c) { (void)s; (void)d; (void)c; }

void inittimer1mhz(void) { }
void uninittimer1mhz(void) { }
long gettime1mhz(void) { return platform_ticks_us() & 0x0fffffff; }
long deltatime1mhz(long b, long a) { a -= b; if (a < 0) a += (1 << 28) - (1 << 16); return a; }

// Text mode output (rep stosw into 0xb8000): no text mode here.
void printchrasm_(long p, long c, long a) { (void)p; (void)c; (void)a; }

