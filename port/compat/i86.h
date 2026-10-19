// Linux stand-in for Watcom's <i86.h>: register structures for int386().
// There are no software interrupts on Linux; int386() only logs the call.
#ifndef __COMPAT_I86_H
#define __COMPAT_I86_H

struct DWORDREGS { unsigned int eax, ebx, ecx, edx, esi, edi, cflag; };
struct WORDREGS  { unsigned short ax, _1, bx, _2, cx, _3, dx, _4, si, _5, di, _6; unsigned int cflag; };
struct BYTEREGS  { unsigned char al, ah; unsigned short _1; unsigned char bl, bh; unsigned short _2;
                   unsigned char cl, ch; unsigned short _3; unsigned char dl, dh; unsigned short _4; };
union REGS { struct DWORDREGS x; struct WORDREGS w; struct BYTEREGS h; };
struct SREGS { unsigned short es, cs, ss, ds, fs, gs; };

int  int386(int intno, union REGS *in, union REGS *out);
int  int386x(int intno, union REGS *in, union REGS *out, struct SREGS *s);
void segread(struct SREGS *s);

// Watcom also accepts the underscore names.
#define _REGS REGS
#define _SREGS SREGS

#define FP_OFF(p) ((unsigned long)(p))
#define FP_SEG(p) (0)
#define MK_FP(seg, off) ((void *)(off))

#endif
