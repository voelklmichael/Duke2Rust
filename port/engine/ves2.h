// PORT: replacement for Ken's VES2.H (VESA 2.0 / DPMI video code).
// The Linux port has a single buffered 8-bit framebuffer ("screen", allocated
// by setgamemode). Showing a page and setting the palette are handed to the
// platform layer. The globals keep their original names and meaning.
// The original file is in the import commit and in duke_nukem_3d/BuildEngine.zip.

#include "platform.h"

static char *screen = NULL;
long xres, yres, bytesperline, frameplace, imageSize, maxpages;
long buffermode, origbuffermode, linearmode;
char permanentupdate = 0, vgacompatible = 1;

#define qlimitrate()

int setvesa(long x, long y)
{
	if (platform_setvideomode(x, y) < 0) return(-1);
	xres = x; yres = y;
	bytesperline = x;
	imageSize = bytesperline*yres;
	maxpages = 1;
	linearmode = 1;
	buffermode = origbuffermode = 1;
	frameplace = FP_OFF(screen);
	return(0);
}

void setactivepage(long dapagenum)
{
	frameplace = FP_OFF(screen);
}

void setvisualpage(long dapagenum)
{
	platform_present((unsigned char *)screen, xres, yres, bytesperline);
}

void uninitvesa()
{
	platform_closevideo();
}

	// dapal: num entries of 4 bytes (blue, green, red, unused), 6 bits per channel.
void VBE_setPalette(long start, long num, char *dapal)
{
	platform_setpalette(start, num, (unsigned char *)dapal);
}

void VBE_getPalette(long start, long num, char *dapal)
{
	platform_getpalette(start, num, (unsigned char *)dapal);
}
