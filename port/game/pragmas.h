//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment

This file is part of Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle, Ken Silverman
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------
// Portable C replacement for the Watcom "#pragma aux" inline assembly that
// Duke Nukem 3D and the Build engine used (original: PRAGMAS.H).
// Semantics follow the original x86 code bit-exactly on a 32-bit target:
// 64-bit intermediates, results truncated to 32 bits.
// Arguments that are addresses are passed as "long" (32-bit, as on DOS); the
// macros cast so callers may pass either pointers or integers.
#ifndef __PRAGMAS_H
#define __PRAGMAS_H

extern long dmval;

long sqr(long a);
long scale(long a, long b, long c);
long mulscale(long a, long b, long s);
long mulscale1(long a, long b);
long mulscale2(long a, long b);
long mulscale3(long a, long b);
long mulscale4(long a, long b);
long mulscale5(long a, long b);
long mulscale6(long a, long b);
long mulscale7(long a, long b);
long mulscale8(long a, long b);
long mulscale9(long a, long b);
long mulscale10(long a, long b);
long mulscale11(long a, long b);
long mulscale12(long a, long b);
long mulscale13(long a, long b);
long mulscale14(long a, long b);
long mulscale15(long a, long b);
long mulscale16(long a, long b);
long mulscale17(long a, long b);
long mulscale18(long a, long b);
long mulscale19(long a, long b);
long mulscale20(long a, long b);
long mulscale21(long a, long b);
long mulscale22(long a, long b);
long mulscale23(long a, long b);
long mulscale24(long a, long b);
long mulscale25(long a, long b);
long mulscale26(long a, long b);
long mulscale27(long a, long b);
long mulscale28(long a, long b);
long mulscale29(long a, long b);
long mulscale30(long a, long b);
long mulscale31(long a, long b);
long mulscale32(long a, long b);
long dmulscale(long a, long b, long c, long d, long s);
long dmulscale1(long a, long b, long c, long d);
long dmulscale2(long a, long b, long c, long d);
long dmulscale3(long a, long b, long c, long d);
long dmulscale4(long a, long b, long c, long d);
long dmulscale5(long a, long b, long c, long d);
long dmulscale6(long a, long b, long c, long d);
long dmulscale7(long a, long b, long c, long d);
long dmulscale8(long a, long b, long c, long d);
long dmulscale9(long a, long b, long c, long d);
long dmulscale10(long a, long b, long c, long d);
long dmulscale11(long a, long b, long c, long d);
long dmulscale12(long a, long b, long c, long d);
long dmulscale13(long a, long b, long c, long d);
long dmulscale14(long a, long b, long c, long d);
long dmulscale15(long a, long b, long c, long d);
long dmulscale16(long a, long b, long c, long d);
long dmulscale17(long a, long b, long c, long d);
long dmulscale18(long a, long b, long c, long d);
long dmulscale19(long a, long b, long c, long d);
long dmulscale20(long a, long b, long c, long d);
long dmulscale21(long a, long b, long c, long d);
long dmulscale22(long a, long b, long c, long d);
long dmulscale23(long a, long b, long c, long d);
long dmulscale24(long a, long b, long c, long d);
long dmulscale25(long a, long b, long c, long d);
long dmulscale26(long a, long b, long c, long d);
long dmulscale27(long a, long b, long c, long d);
long dmulscale28(long a, long b, long c, long d);
long dmulscale29(long a, long b, long c, long d);
long dmulscale30(long a, long b, long c, long d);
long dmulscale31(long a, long b, long c, long d);
long dmulscale32(long a, long b, long c, long d);
long tmulscale1(long a, long b, long c, long d, long e, long f);
long tmulscale2(long a, long b, long c, long d, long e, long f);
long tmulscale3(long a, long b, long c, long d, long e, long f);
long tmulscale4(long a, long b, long c, long d, long e, long f);
long tmulscale5(long a, long b, long c, long d, long e, long f);
long tmulscale6(long a, long b, long c, long d, long e, long f);
long tmulscale7(long a, long b, long c, long d, long e, long f);
long tmulscale8(long a, long b, long c, long d, long e, long f);
long tmulscale9(long a, long b, long c, long d, long e, long f);
long tmulscale10(long a, long b, long c, long d, long e, long f);
long tmulscale11(long a, long b, long c, long d, long e, long f);
long tmulscale12(long a, long b, long c, long d, long e, long f);
long tmulscale13(long a, long b, long c, long d, long e, long f);
long tmulscale14(long a, long b, long c, long d, long e, long f);
long tmulscale15(long a, long b, long c, long d, long e, long f);
long tmulscale16(long a, long b, long c, long d, long e, long f);
long tmulscale17(long a, long b, long c, long d, long e, long f);
long tmulscale18(long a, long b, long c, long d, long e, long f);
long tmulscale19(long a, long b, long c, long d, long e, long f);
long tmulscale20(long a, long b, long c, long d, long e, long f);
long tmulscale21(long a, long b, long c, long d, long e, long f);
long tmulscale22(long a, long b, long c, long d, long e, long f);
long tmulscale23(long a, long b, long c, long d, long e, long f);
long tmulscale24(long a, long b, long c, long d, long e, long f);
long tmulscale25(long a, long b, long c, long d, long e, long f);
long tmulscale26(long a, long b, long c, long d, long e, long f);
long tmulscale27(long a, long b, long c, long d, long e, long f);
long tmulscale28(long a, long b, long c, long d, long e, long f);
long tmulscale29(long a, long b, long c, long d, long e, long f);
long tmulscale30(long a, long b, long c, long d, long e, long f);
long tmulscale31(long a, long b, long c, long d, long e, long f);
long tmulscale32(long a, long b, long c, long d, long e, long f);
long boundmulscale(long a, long b, long s);
long divscale(long a, long b, long s);
long divscale1(long a, long b);
long divscale2(long a, long b);
long divscale3(long a, long b);
long divscale4(long a, long b);
long divscale5(long a, long b);
long divscale6(long a, long b);
long divscale7(long a, long b);
long divscale8(long a, long b);
long divscale9(long a, long b);
long divscale10(long a, long b);
long divscale11(long a, long b);
long divscale12(long a, long b);
long divscale13(long a, long b);
long divscale14(long a, long b);
long divscale15(long a, long b);
long divscale16(long a, long b);
long divscale17(long a, long b);
long divscale18(long a, long b);
long divscale19(long a, long b);
long divscale20(long a, long b);
long divscale21(long a, long b);
long divscale22(long a, long b);
long divscale23(long a, long b);
long divscale24(long a, long b);
long divscale25(long a, long b);
long divscale26(long a, long b);
long divscale27(long a, long b);
long divscale28(long a, long b);
long divscale29(long a, long b);
long divscale30(long a, long b);
long divscale31(long a, long b);
long divscale32(long a, long b);

long mul3(long a);
long mul5(long a);
long mul9(long a);
long divmod(long a, long b);
long moddiv(long a, long b);
long klabs(long a);
long ksgn(long a);
long umin(long a, long b);
long umax(long a, long b);
long kmin(long a, long b);
long kmax(long a, long b);


// Memory helpers. Counts: clearbuf/copybuf in dwords, *byte variants in bytes.
void clearbuf_(long d, long c, long a);
void clearbufbyte_(long d, long c, long a);
void copybuf_(long s, long d, long c);
void copybufbyte_(long s, long d, long c);
void copybufreverse_(long s, long d, long c);
void qinterpolatedown16_(long bufptr, long num, long val, long add);
void qinterpolatedown16short_(long bufptr, long num, long val, long add);
#define clearbuf(d,c,a)       clearbuf_((long)(d),(long)(c),(long)(a))
#define clearbufbyte(d,c,a)   clearbufbyte_((long)(d),(long)(c),(long)(a))
#define copybuf(s,d,c)        copybuf_((long)(s),(long)(d),(long)(c))
#define copybufbyte(s,d,c)    copybufbyte_((long)(s),(long)(d),(long)(c))
#define copybufreverse(s,d,c) copybufreverse_((long)(s),(long)(d),(long)(c))
#define qinterpolatedown16(b,n,v,a)      qinterpolatedown16_((long)(b),(long)(n),(long)(v),(long)(a))
#define qinterpolatedown16short(b,n,v,a) qinterpolatedown16short_((long)(b),(long)(n),(long)(v),(long)(a))

void swapchar_(long a, long b);
void swapshort_(long a, long b);
void swaplong_(long a, long b);
void swapbuf4_(long a, long b, long c);
void swap64bit_(long a, long b);
void swapchar2_(long a, long b, long xsiz);
#define swapchar(a,b)      swapchar_((long)(a),(long)(b))
#define swapshort(a,b)     swapshort_((long)(a),(long)(b))
#define swaplong(a,b)      swaplong_((long)(a),(long)(b))
#define swapbuf4(a,b,c)    swapbuf4_((long)(a),(long)(b),(long)(c))
#define swap64bit(a,b)     swap64bit_((long)(a),(long)(b))
#define swapchar2(a,b,x)   swapchar2_((long)(a),(long)(b),(long)(x))

long readpixel_(long p);
void drawpixel_(long p, long c);
void drawpixels_(long p, long c);
void drawpixelses_(long p, long c);
#define readpixel(p)       readpixel_((long)(p))
#define drawpixel(p,c)     drawpixel_((long)(p),(long)(c))
#define drawpixels(p,c)    drawpixels_((long)(p),(long)(c))
#define drawpixelses(p,c)  drawpixelses_((long)(p),(long)(c))


// Hardware access (DOS: interrupts, I/O ports, VGA, PIT). Routed to the platform layer.
void int5(void);
void setvmode(long mode);
long setupmouse(void);
void readmousexy_(long x, long y);
void readmousebstatus_(long b);
#define readmousexy(x,y)     readmousexy_((long)(x),(long)(y))
#define readmousebstatus(b)  readmousebstatus_((long)(b))
void koutp(long port, long val);
void koutpw(long port, long val);
long kinp(long port);
void limitrate(void);
long readtimer(void);
void setcolor16(long col);
void vlin16first_(long p, long c);
void vlin16_(long p, long c);
void drawpixel16(long p);
void fillscreen16_(long p, long col, long c);
#define vlin16first(p,c)      vlin16first_((long)(p),(long)(c))
#define vlin16(p,c)           vlin16_((long)(p),(long)(c))
#define fillscreen16(p,col,c) fillscreen16_((long)(p),(long)(col),(long)(c))
void redblueblit_(long a, long b, long c);
void chainblit_(long s, long d, long c);
#define redblueblit(a,b,c)  redblueblit_((long)(a),(long)(b),(long)(c))
#define chainblit(s,d,c)    chainblit_((long)(s),(long)(d),(long)(c))
void inittimer1mhz(void);
void uninittimer1mhz(void);
long gettime1mhz(void);
long deltatime1mhz(long b, long a);
void printchrasm_(long p, long c, long a);
#define printchrasm(p,c,a) printchrasm_((long)(p),(long)(c),(long)(a))

#endif

