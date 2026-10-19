// PORT: headless platform layer (no window, no input, no sound).
// Useful for automated runs: with DUKE_FRAMEDUMP=<dir>, every
// DUKE_FRAMEDUMP_EVERY-th presented frame (default 30) is written as a PPM.
#include <time.h>
#include "compat.h"
#include "platform.h"

static long vidx, vidy;
static unsigned char palette[256 * 4];
static long framecount = 0;

int platform_setvideomode(long xdim, long ydim)
{
	vidx = xdim; vidy = ydim;
	fprintf(stderr, "platform: video mode %ldx%ld (headless)\n", xdim, ydim);
	return 0;
}

void platform_closevideo(void) { }

	// Writes frame as a binary PPM using the current palette (6-bit -> 8-bit).
static void dumpframe(const unsigned char *frame, long xdim, long ydim, long pitch, const char *dir)
{
	char name[1024];
	FILE *f;
	long x, y;

	snprintf(name, sizeof(name), "%s/frame%06ld.ppm", dir, framecount);
	if ((f = fopen(name, "wb")) == NULL) return;
	fprintf(f, "P6\n%ld %ld\n255\n", xdim, ydim);
	for (y = 0; y < ydim; y++)
		for (x = 0; x < xdim; x++)
		{
			const unsigned char *p = &palette[frame[y * pitch + x] * 4];
			unsigned char rgb[3] = { (unsigned char)(p[2] << 2), (unsigned char)(p[1] << 2), (unsigned char)(p[0] << 2) };
			fwrite(rgb, 1, 3, f);
		}
	fclose(f);
}

void platform_present(const unsigned char *frame, long xdim, long ydim, long pitch)
{
	const char *dir = getenv("DUKE_FRAMEDUMP");
	const char *every = getenv("DUKE_FRAMEDUMP_EVERY");
	long n = every ? atol(every) : 30;

	if (dir && n > 0 && (framecount % n) == 0) dumpframe(frame, xdim, ydim, pitch, dir);
	framecount++;
}

void platform_setpalette(long start, long num, const unsigned char *pal)
{
	if (start < 0 || start + num > 256) return;
	memcpy(&palette[start * 4], pal, num * 4);
}

void platform_getpalette(long start, long num, unsigned char *pal)
{
	if (start < 0 || start + num > 256) return;
	memcpy(pal, &palette[start * 4], num * 4);
}

void platform_setvmode(long mode) { (void)mode; }

int  platform_mousepresent(void) { return 0; }
void platform_readmousexy(short *dx, short *dy) { *dx = 0; *dy = 0; }
int  platform_readmousebuttons(void) { return 0; }

void platform_outp(long port, long val) { (void)port; (void)val; }
long platform_inp(long port) { (void)port; return 0; }

unsigned long platform_ticks_us(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (unsigned long)((unsigned long long)ts.tv_sec * 1000000ull + ts.tv_nsec / 1000);
}

void platform_sleep_ms(unsigned ms)
{
	struct timespec ts = { ms / 1000, (long)(ms % 1000) * 1000000L };
	nanosleep(&ts, NULL);
}

int platform_kbhit(void) { return 0; }
int platform_getch(void) { return 13; }
