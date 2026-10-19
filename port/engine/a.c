// PORT: C replacement for Ken's A.ASM (the texture mapping inner loops).
//
// The assembly was self-modifying: the setup* functions patched constants
// (shift counts, table addresses, increments) into the loop code. Here those
// constants are static variables. Each function documents its original
// register interface; the C argument order matches it (see a.h).
//
// Differences from the assembly, all affecting rendering only (never game state):
// - Several fast paths approximated the exact per-pixel stepping to save
//   registers (hlineasm4's 4-pixel loop, vlineasm4, mvlineasm4, qrhlineasm4).
//   This version always does the exact per-pixel calculation.
// - Texture index shifts use 64-bit intermediates, so a shift by 32 (tiles of
//   size 1) yields 0 instead of x86's "shift count mod 32".
// - mhlineskipmodify/thlineskipmodify reused constants from the previous call;
//   they are not called by the engine and here behave like mhline/thline.

#include <string.h>
#include "a.h"

typedef unsigned long long u64;

extern long asm1, asm2, asm3, asm4;
extern long vplce[4], vince[4], palookupoffse[4], bufplce[4];
extern long globalx3, globaly3;
extern long reciptable[2048];
extern long ylookup[];

	// Texel index into a column-major tile of (1<<logx) by (1<<logy):
	// ((x >> (32-logx)) << logy) + (y >> (32-logy))
#define TILEIDX(x, y, logx, logy) \
	((unsigned long)((((u64)(unsigned long)(x) >> (32 - (logx))) << (logy)) + ((u64)(unsigned long)(y) >> (32 - (logy)))))

	// vplc >> shift, where shift may be 32.
#define VSHR(v, shift) ((unsigned long)((u64)(unsigned long)(v) >> (shift)))

static long bpl;                  // bytes per line (setvlinebpl)
static unsigned char *gtrans;     // translucency table (fixtransluscence)
static long transrev = 0;         // 0: transluc[dest*256+src], 1: transluc[src*256+dest]

static unsigned char trans(unsigned char dest, unsigned char src)
{
	if (transrev) return gtrans[(src << 8) + dest];
	return gtrans[(dest << 8) + src];
}

	// CPU detection that switched to Pentium Pro / II versions of a few routines.
void mmxoverlay(void) { }

void setvlinebpl(long dabpl) { bpl = dabpl; }
void fixtransluscence(long datransoff) { gtrans = (unsigned char *)datransoff; }
void settransnormal(void) { transrev = 0; }
void settransreverse(void) { transrev = 1; }

//---------------------------------------------------------------------------
// Horizontal lines for floors/ceilings (power-of-2 tiles).
//---------------------------------------------------------------------------

static long hlogx, hlogy, hbxinc, hbyinc;
static unsigned char *hbuf, *hpal;

	// eax=logx, ebx=logy, ecx=bufplc
void sethlinesizes(long logx, long logy, long bufplc)
	{ hlogx = logx; hlogy = logy; hbuf = (unsigned char *)bufplc; }

void setpalookupaddress(char *paladdr) { hpal = (unsigned char *)paladdr; }

	// Increments used when hlineasm4 is called with skiploadincs set.
void setuphlineasm4(long bxinc, long byinc) { hbxinc = bxinc; hbyinc = byinc; }

	// eax=cnt, ebx=skiploadincs, ecx=paloffs, edx=by, esi=bx, edi=p
	// Draws cnt+1 pixels from p leftwards; bx/by step by -asm1/-asm2.
void hlineasm4(long cnt, long skiploadincs, long paloffs, unsigned long by, unsigned long bx, long p)
{
	unsigned char *pp = (unsigned char *)p;
	long bxinc = asm1, byinc = asm2;

	if (skiploadincs < 0) { bxinc = hbxinc; byinc = hbyinc; }
	for (; cnt >= 0; cnt--)
	{
		unsigned long t = hbuf[TILEIDX(bx, by, hlogx, hlogy)];
		*pp-- = hpal[(paloffs & ~255) | t];
		bx -= bxinc;
		by -= byinc;
	}
}

//---------------------------------------------------------------------------
// Horizontal lines for rotatesprite (non power-of-2 tiles).
// The texture pointer steps by bufinc (+1 on a y borrow, +ysiz on an x borrow).
//---------------------------------------------------------------------------

static unsigned long rbxinc, rbyinc;
static long rbufinc, rysiz;
static unsigned char *rpal;

	// eax=bxinc, ebx=byinc, ecx=bufinc, edx=paloffs, esi=ysiz
void setuprhlineasm4(long bxinc, long byinc, long bufinc, long paloffs, long ysiz, long junk)
{
	rbxinc = bxinc; rbyinc = byinc; rbufinc = bufinc;
	rpal = (unsigned char *)paloffs; rysiz = ysiz;
}

	// eax=cnt, ebx=bufplc, edx=bx, esi=by, edi=p
	// Draws cnt pixels at p-1, p-2, ... p-cnt.
void rhlineasm4(long cnt, long bufplc, long junk, unsigned long bx, unsigned long by, long p)
{
	unsigned char *pp = (unsigned char *)p;
	unsigned char *b = (unsigned char *)bufplc;

	for (; cnt > 0; cnt--)
	{
		unsigned char t = *b;
		long borrowx = (bx < rbxinc), borrowy = (by < rbyinc);
		bx -= rbxinc; by -= rbyinc;
		b -= rbufinc + borrowy;
		if (borrowx) b -= rysiz;
		*--pp = rpal[t];
	}
}

static unsigned long rmbxinc, rmbyinc;
static long rmbufinc, rmysiz;
static unsigned char *rmpal;

void setuprmhlineasm4(long bxinc, long byinc, long bufinc, long paloffs, long ysiz, long junk)
{
	rmbxinc = bxinc; rmbyinc = byinc; rmbufinc = bufinc;
	rmpal = (unsigned char *)paloffs; rmysiz = ysiz;
}

	// Masked version of rhlineasm4: texel 255 is transparent.
void rmhlineasm4(long cnt, long bufplc, long junk, unsigned long bx, unsigned long by, long p)
{
	unsigned char *pp = (unsigned char *)p;
	unsigned char *b = (unsigned char *)bufplc;

	for (; cnt > 0; cnt--)
	{
		unsigned char t = *b;
		long borrowx = (bx < rmbxinc), borrowy = (by < rmbyinc);
		bx -= rmbxinc; by -= rmbyinc;
		b -= rmbufinc + borrowy;
		if (borrowx) b -= rmysiz;
		--pp;
		if (t != 255) *pp = rmpal[t];
	}
}

static unsigned long qrbyinc;
static long qrbufinc;
static unsigned char *qrpal;

	// ebx=byinc, ecx=bufinc, edx=paloffs (no x increment: x never changes)
void setupqrhlineasm4(long bxinc, long byinc, long bufinc, long paloffs, long ysiz, long junk)
{
	qrbyinc = byinc; qrbufinc = bufinc; qrpal = (unsigned char *)paloffs;
}

void qrhlineasm4(long cnt, long bufplc, long junk, unsigned long bx, unsigned long by, long p)
{
	unsigned char *pp = (unsigned char *)p;
	unsigned char *b = (unsigned char *)bufplc;

	for (; cnt > 0; cnt--)
	{
		unsigned char t = *b;
		long borrowy = (by < qrbyinc);
		by -= qrbyinc;
		b -= qrbufinc + borrowy;
		*--pp = qrpal[t];
	}
}

//---------------------------------------------------------------------------
// Vertical lines for walls. Texel = buf[vplc >> shift]; the palette lookup
// keeps the bits above the low byte of the shifted index, as the assembly did
// ("mov bl, [buf+ebx]" then "mov bl, [pal+ebx]").
//---------------------------------------------------------------------------

static long vshift, mvshift, tvshift;

void setupvlineasm(long neglogy) { vshift = neglogy & 255; }

	// eax=vinc, ebx=paloffs, ecx=cnt, edx=vplc, esi=bufplc, edi=p
	// Draws one pixel if cnt == 0, otherwise like vlineasm1. Returns vplc+vinc.
long prevlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p)
{
	unsigned long i;

	if (cnt != 0) return vlineasm1(vinc, paloffs, cnt, vplc, bufplc, p);
	i = VSHR(vplc, vshift);
	*(unsigned char *)p = ((unsigned char *)paloffs)[(i & ~255) | ((unsigned char *)bufplc)[i]];
	return (long)(vplc + vinc);
}

	// Draws cnt+1 pixels downwards from p. Returns the new vplc.
long vlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p)
{
	unsigned char *pal = (unsigned char *)paloffs, *buf = (unsigned char *)bufplc;
	unsigned char *pp = (unsigned char *)p;

	for (cnt++; cnt != 0; cnt--)
	{
		unsigned long i = VSHR(vplc, vshift);
		*pp = pal[(i & ~255) | buf[i]];
		vplc += vinc;
		pp += bpl;
	}
	return (long)vplc;
}

	// ecx=cnt, edi=p. Draws cnt rows of 4 adjacent columns using
	// vplce/vince/bufplce/palookupoffse[0..3]; updates vplce.
void vlineasm4(long cnt, long p)
{
	unsigned char *pp = (unsigned char *)p;
	long i;

	for (; cnt > 0; cnt--)
	{
		for (i = 0; i < 4; i++)
		{
			unsigned long j = VSHR(vplce[i], vshift);
			pp[i] = ((unsigned char *)palookupoffse[i])[(j & ~255) | ((unsigned char *)bufplce[i])[j]];
			vplce[i] += vince[i];
		}
		pp += bpl;
	}
}

void setupmvlineasm(long neglogy) { mvshift = neglogy & 255; }

	// Masked: texel 255 is transparent. Draws cnt+1 pixels. Returns the new vplc.
long mvlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p)
{
	unsigned char *pal = (unsigned char *)paloffs, *buf = (unsigned char *)bufplc;
	unsigned char *pp = (unsigned char *)p;

	for (; cnt >= 0; cnt--)
	{
		unsigned long i = VSHR(vplc, mvshift);
		unsigned char t = buf[i];
		if (t != 255) *pp = pal[(i & ~255) | t];
		vplc += vinc;
		pp += bpl;
	}
	return (long)vplc;
}

	// Masked version of vlineasm4.
void mvlineasm4(long cnt, long p)
{
	unsigned char *pp = (unsigned char *)p;
	long i;

	for (cnt &= 0xffff; cnt > 0; cnt--)
	{
		for (i = 0; i < 4; i++)
		{
			unsigned long j = VSHR(vplce[i], mvshift);
			unsigned char t = ((unsigned char *)bufplce[i])[j];
			if (t != 255) pp[i] = ((unsigned char *)palookupoffse[i])[(j & ~255) | t];
			vplce[i] += vince[i];
		}
		pp += bpl;
	}
}

void setuptvlineasm(long neglogy) { tvshift = neglogy & 255; }

	// Masked and translucent. Draws cnt+1 pixels. Returns the new vplc.
long tvlineasm1(long vinc, long paloffs, long cnt, unsigned long vplc, long bufplc, long p)
{
	unsigned char *pal = (unsigned char *)paloffs, *buf = (unsigned char *)bufplc;
	unsigned char *pp = (unsigned char *)p;

	for (cnt++; cnt != 0; cnt--)
	{
		unsigned long i = VSHR(vplc, tvshift);
		unsigned char t = buf[i];
		if (t != 255) *pp = trans(*pp, pal[(i & ~255) | t]);
		vplc += vinc;
		pp += bpl;
	}
	return (long)vplc;
}

static long tv2shift;
static unsigned char *tv2pal1, *tv2pal2;

	// eax=shiftval, ebx=palookup1, ecx=palookup2
void setuptvlineasm2(long neglogy, long paloffs1, long paloffs2)
{
	tv2shift = neglogy & 255;
	tv2pal1 = (unsigned char *)paloffs1;
	tv2pal2 = (unsigned char *)paloffs2;
}

	// eax=vplc2, ebx=vinc1, ecx=bufplc1, edx=bufplc2, esi=vplc1, edi=p
	// asm1=vinc2, asm2=pend. Draws columns p and p+1 (translucent, masked)
	// from p down while below pend. Returns asm1=vplc1, asm2=vplc2.
void tvlineasm2(unsigned long vplc2, long vinc1, long bufplc1, long bufplc2, unsigned long vplc1, long p)
{
	unsigned char *buf1 = (unsigned char *)bufplc1, *buf2 = (unsigned char *)bufplc2;
	long vinc2 = asm1, pend = asm2;

	do
	{
		unsigned char *pp = (unsigned char *)p;
		unsigned char t1 = buf1[VSHR(vplc1, tv2shift)];
		unsigned char t2 = buf2[VSHR(vplc2, tv2shift)];
		vplc1 += vinc1;
		vplc2 += vinc2;
		if (t1 != 255) pp[0] = trans(pp[0], tv2pal1[t1]);
		if (t2 != 255) pp[1] = trans(pp[1], tv2pal2[t2]);
		p += bpl;
	} while (p < pend);

	asm1 = (long)vplc1;
	asm2 = (long)vplc2;
}

//---------------------------------------------------------------------------
// Vertical lines for rotatesprite (non power-of-2 tiles).
// by/bx are 16.16 fractions; the texture pointer steps by yinc's whole part
// plus bxinc ((xv>>16)*ysiz), +1 on a y fraction carry, +ysiz on an x carry.
//---------------------------------------------------------------------------

typedef struct { unsigned char *pal; unsigned long yfracinc, xfracinc; long step, stepx; } spritevlinetype;
static spritevlinetype sv, msv, tsv;

	// eax=paloffs, ebx=bxinc, ecx=byinc (x fraction inc), edx=ysiz, esi=yinc
static void setupsv(spritevlinetype *s, long paloffs, long bxinc, long byinc, long ysiz, long yinc)
{
	s->pal = (unsigned char *)paloffs;
	s->yfracinc = (unsigned long)yinc << 16;
	s->step = (yinc >> 16) + bxinc;
	s->stepx = s->step + ysiz;
	s->xfracinc = byinc;
}

void setupspritevline(long paloffs, long bxinc, long byinc, long ysiz, long yinc, long junk)
	{ setupsv(&sv, paloffs, bxinc, byinc, ysiz, yinc); }
void msetupspritevline(long paloffs, long bxinc, long byinc, long ysiz, long yinc, long junk)
	{ setupsv(&msv, paloffs, bxinc, byinc, ysiz, yinc); }
void tsetupspritevline(long paloffs, long bxinc, long byinc, long ysiz, long yinc, long junk)
	{ setupsv(&tsv, paloffs, bxinc, byinc, ysiz, yinc); }

	// ebx=by (y fraction), ecx=cnt, edx=bx (x fraction), esi=bufplc, edi=p
	// mode: 0 normal, 1 masked, 2 masked+translucent. Draws cnt-1 pixels.
static void spritevlinecommon(spritevlinetype *s, int mode, unsigned long by, long cnt, unsigned long bx, long bufplc, long p)
{
	unsigned char *b = (unsigned char *)bufplc, *pp = (unsigned char *)p;

	for (;;)
	{
		long carryx, carryy;
		unsigned char t;

		carryx = (bx + s->xfracinc < bx); bx += s->xfracinc;
		if (--cnt == 0) break;
		carryy = (by + s->yfracinc < by); by += s->yfracinc;
		t = *b;
		b += (carryx ? s->stepx : s->step) + carryy;
		if (mode == 0) *pp = s->pal[t];
		else if (t != 255)
		{
			if (mode == 1) *pp = s->pal[t];
			else *pp = trans(*pp, s->pal[t]);
		}
		pp += bpl;
	}
}

void spritevline(long junk, unsigned long by, long cnt, unsigned long bx, long bufplc, long p)
	{ spritevlinecommon(&sv, 0, by, cnt, bx, bufplc, p); }
void mspritevline(long junk, unsigned long by, long cnt, unsigned long bx, long bufplc, long p)
	{ spritevlinecommon(&msv, 1, by, cnt, bx, bufplc, p); }
void tspritevline(long junk, unsigned long by, long cnt, unsigned long bx, long bufplc, long p)
	{ spritevlinecommon(&tsv, 2, by, cnt, bx, bufplc, p); }

//---------------------------------------------------------------------------
// Masked / translucent horizontal lines (sloped/masked floors, map view).
// asm1=bxinc, asm2=byinc, asm3=shade palette.
//---------------------------------------------------------------------------

static long mlogx, mlogy, tlogx, tlogy;

void msethlineshift(long logx, long logy) { mlogx = logx & 31; mlogy = logy & 31; }
void tsethlineshift(long logx, long logy) { tlogx = logx & 31; tlogy = logy & 31; }

	// eax=bufplc, ebx=bx, ecx=cnt<<16, esi=by, edi=p
	// Draws (cntup16>>16)+1 pixels from p rightwards; texel 255 is transparent.
void mhline(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p)
{
	unsigned char *buf = (unsigned char *)bufplc, *pal = (unsigned char *)asm3;
	unsigned char *pp = (unsigned char *)p;
	long cnt;

	for (cnt = (unsigned long)cntup16 >> 16; cnt >= 0; cnt--)
	{
		unsigned char t = buf[TILEIDX(bx, by, mlogx, mlogy)];
		if (t != 255) *pp = pal[t];
		bx += asm1;
		by += asm2;
		pp++;
	}
}

void mhlineskipmodify(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p)
	{ mhline(bufplc, bx, cntup16, junk, by, p); }

	// Translucent version of mhline.
void thline(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p)
{
	unsigned char *buf = (unsigned char *)bufplc, *pal = (unsigned char *)asm3;
	unsigned char *pp = (unsigned char *)p;
	long cnt;

	for (cnt = (unsigned long)cntup16 >> 16; cnt >= 0; cnt--)
	{
		unsigned char t = buf[TILEIDX(bx, by, tlogx, tlogy)];
		if (t != 255) *pp = trans(*pp, pal[t]);
		bx += asm1;
		by += asm2;
		pp++;
	}
}

void thlineskipmodify(long bufplc, unsigned long bx, long cntup16, long junk, unsigned long by, long p)
	{ thline(bufplc, bx, cntup16, junk, by, p); }

//---------------------------------------------------------------------------
// Sloped floors/ceilings: vertical lines with perspective correction every
// 8 pixels. The assembly kept the 1/z accumulator on the FPU stack and walked
// the slopalookup table (shade palettes) downwards using ESP as the pointer.
//---------------------------------------------------------------------------

static unsigned char *slopebuf;
static long slopepinc, slopemask, slopeshx, slopeshy;
static float slopezinc;

	// eax: logx | (logy<<8), ebx=bufplc, ecx=pinc (bytes per pixel step)
	// asm1 (integer) is the 1/z increment per 8 pixels; stored as float.
void setupslopevlin(long logylogx, long bufplc, long pinc)
{
	long logx = logylogx & 255, logy = (logylogx >> 8) & 255;

	slopebuf = (unsigned char *)bufplc;
	slopepinc = pinc;
	slopemask = (long)((((u64)1 << logx) - 1) << logy);
	slopeshy = 32 - logy;
	slopeshx = 32 - logy - logx;
	slopezinc = (float)asm1;
	memcpy(&asm2, &slopezinc, 4);   // the assembly stored it in asm2 as a float
}

	// eax=p, ebx=i (1/z start), ecx=slopalookup pointer, edx=cnt, esi=bx, edi=by
	// asm3 (integer): 1/z start for the FPU accumulator. Draws cnt pixels from p
	// stepping by pinc (upwards on screen).
void slopevlin(long p, long i, long slopaloffs, long cnt, long bx, long by)
{
	long *slopalptr = (long *)slopaloffs;
	unsigned char *pp = (unsigned char *)p - slopepinc;
	unsigned long ubx, uby;
	long prevrecip, n;
	double z;

	z = (double)asm3 + (double)slopezinc;
	prevrecip = i;
	ubx = (unsigned long)bx + (unsigned long)globalx3 * ((unsigned long)i << 3);
	uby = (unsigned long)by + (unsigned long)globaly3 * ((unsigned long)i << 3);

	for (;;)
	{
		float zf = (float)z;
		unsigned long u, r, xinc, yinc;
		long d;

		memcpy(&u, &zf, 4);
		r = ((unsigned long)reciptable[(u & 0x007ff000) >> 12] >> ((((u << 1) >> 24) - 2) & 31)) ^ (unsigned long)((long)u >> 31);
		d = (long)r - prevrecip;
		prevrecip = (long)r;
		xinc = (unsigned long)globalx3 * (unsigned long)d;
		yinc = (unsigned long)d * (unsigned long)globaly3;
		z += (double)slopezinc;
		asm4 = cnt;

		for (n = (cnt < 8) ? cnt : 8; n > 0; n--)
		{
			unsigned long ix = (ubx >> slopeshx) & slopemask;
			unsigned long iy = (unsigned long)((u64)uby >> slopeshy);
			unsigned char *pal;
			ubx += xinc;
			uby += yinc;
			pp += slopepinc;
			iy = (iy & ~255) | slopebuf[ix + iy];
			pal = (unsigned char *)*slopalptr--;
			*pp = pal[iy];
		}

		cnt -= 8;
		if (cnt <= 0) break;
	}
	asm1 = prevrecip;
}

//---------------------------------------------------------------------------
// Voxels.
//---------------------------------------------------------------------------

static long slabbpl;
static unsigned char *slabpal;

	// eax=bytes per line, ebx=palette
void setupdrawslab(long dabpl, long pal) { slabbpl = dabpl; slabpal = (unsigned char *)pal; }

	// eax=dx (width), ebx=v (16.16), ecx=dy (rows), edx=vi, esi=vptr, edi=p
	// Fills dy rows of dx pixels with pal[vptr[v>>16]]. dx 0 draws 1 pixel.
void drawslab(long dx, long v, long dy, long vi, long vptr, long p)
{
	unsigned char *pp = (unsigned char *)p, *vp = (unsigned char *)vptr;
	unsigned long uv = (unsigned long)v;
	unsigned long width = ((unsigned long)dx < 2) ? 1 : (unsigned long)dx;

	for (; dy > 0; dy--)
	{
		memset(pp, slabpal[vp[uv >> 16]], width);
		uv += vi;
		pp += slabbpl;
	}
}
