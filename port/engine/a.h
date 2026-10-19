// PORT: C replacement for Ken's A.ASM (the texture mapping inner loops).
// Prototypes for a.c. The original functions took their arguments in
// registers (Watcom "#pragma aux ... parm [eax][ebx][ecx][edx][esi][edi]");
// the argument order here is the same as that register order.
#ifndef __A_H
#define __A_H

void mmxoverlay(void);

void sethlinesizes(long logx, long logy, long bufplc);
void setpalookupaddress(char *paladdr);
void setuphlineasm4(long bxinc, long byinc);
void hlineasm4(long cnt, long skiploadincs, long paloffs, unsigned long by, unsigned long bx, long p);

void setuprhlineasm4(long bxinc, long byinc, long bufinc, long paloffs, long ysiz, long junk);
void rhlineasm4(long cnt, long bufplc, long junk, unsigned long bx, unsigned long by, long p);
void setuprmhlineasm4(long bxinc, long byinc, long bufinc, long paloffs, long ysiz, long junk);
void rmhlineasm4(long cnt, long bufplc, long junk, unsigned long bx, unsigned long by, long p);
void setupqrhlineasm4(long bxinc, long byinc, long bufinc, long paloffs, long ysiz, long junk);
void qrhlineasm4(long cnt, long bufplc, long junk, unsigned long bx, unsigned long by, long p);

void setvlinebpl(long dabpl);
void fixtransluscence(long datransoff);
void settransnormal(void);
void settransreverse(void);

void setupvlineasm(long neglogy);
long prevlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p);
long vlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p);
void vlineasm4(long cnt, long p);

void setupmvlineasm(long neglogy);
long mvlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p);
void mvlineasm4(long cnt, long p);

void setuptvlineasm(long neglogy);
long tvlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p);
void setuptvlineasm2(long neglogy, long paloffs1, long paloffs2);
void tvlineasm2(unsigned long vplc2, long vinc1, long bufplc1, long bufplc2, unsigned long vplc1, long p);

void setupspritevline(long paloffs, long bxinc, long byinc, long ysiz, long yinc, long junk);
void spritevline(long junk, unsigned long by, long cnt, unsigned long bx, long bufplc, long p);
void msetupspritevline(long paloffs, long bxinc, long byinc, long ysiz, long yinc, long junk);
void mspritevline(long junk, unsigned long by, long cnt, unsigned long bx, long bufplc, long p);
void tsetupspritevline(long paloffs, long bxinc, long byinc, long ysiz, long yinc, long junk);
void tspritevline(long junk, unsigned long by, long cnt, unsigned long bx, long bufplc, long p);

void msethlineshift(long logx, long logy);
void mhline(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p);
void mhlineskipmodify(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p);
void tsethlineshift(long logx, long logy);
void thline(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p);
void thlineskipmodify(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p);

void setupslopevlin(long logylogx, long bufplc, long pinc);
void slopevlin(long p, long i, long slopaloffs, long cnt, long bx, long by);

void setupdrawslab(long dabpl, long pal);
void drawslab(long dx, long v, long dy, long vi, long vptr, long p);

#endif
