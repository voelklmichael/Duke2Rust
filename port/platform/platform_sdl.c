// PORT: SDL2 platform layer.
//
// On DOS the keyboard and timer were interrupt driven, asynchronous to the
// game, and the game polls their state in busy loops. To keep that model, one
// "platform thread" owns SDL: it handles window events, translates keys into
// MACT scancodes (KB_KeyEvent, like the keyboard interrupt), accumulates mouse
// motion and shows the most recent frame. The game thread only hands over
// frames and palettes.
//
// Environment: DUKE_FRAMEDUMP / DUKE_FRAMEDUMP_EVERY as in platform_null.c,
// SDL_VIDEODRIVER=dummy for headless runs.
#include <SDL.h>
#include <time.h>
#include "compat.h"
#include "platform.h"

void KB_KeyEvent(int scancode, long keypressed);   // mact/keyboard.c

static SDL_Thread *thread;
static SDL_mutex *lock;
static SDL_cond *cond;

static volatile int running = 0, quitrequested = 0;
static volatile long wantx = 0, wanty = 0;   // requested video mode (0: none)
static long curx = 0, cury = 0;              // mode of the current window

static unsigned char frame[1600 * 1200];     // last presented frame (8-bit)
static long framex, framey;
static volatile int framedirty = 0;
static unsigned char palette[256 * 4];       // b, g, r, unused (6-bit)

static volatile int mousedx = 0, mousedy = 0, mousebuttons = 0;
static volatile int keyhit = 0;

static long framecount = 0;

//---------------------------------------------------------------------------
// Key translation: SDL scancode -> MACT scancode (see game/keyboard.h).
//---------------------------------------------------------------------------

static int mactscancode(SDL_Scancode s)
{
	if (s >= SDL_SCANCODE_A && s <= SDL_SCANCODE_Z)
	{
		static const unsigned char letters[26] =
		{
			0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
			0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c
		};
		return letters[s - SDL_SCANCODE_A];
	}
	if (s >= SDL_SCANCODE_1 && s <= SDL_SCANCODE_9) return 0x02 + (s - SDL_SCANCODE_1);
	if (s >= SDL_SCANCODE_F1 && s <= SDL_SCANCODE_F10) return 0x3b + (s - SDL_SCANCODE_F1);

	switch (s)
	{
		case SDL_SCANCODE_0: return 0x0b;
		case SDL_SCANCODE_RETURN: return 0x1c;
		case SDL_SCANCODE_ESCAPE: return 0x01;
		case SDL_SCANCODE_BACKSPACE: return 0x0e;
		case SDL_SCANCODE_TAB: return 0x0f;
		case SDL_SCANCODE_SPACE: return 0x39;
		case SDL_SCANCODE_MINUS: return 0x0c;
		case SDL_SCANCODE_EQUALS: return 0x0d;
		case SDL_SCANCODE_LEFTBRACKET: return 0x1a;
		case SDL_SCANCODE_RIGHTBRACKET: return 0x1b;
		case SDL_SCANCODE_BACKSLASH: return 0x2b;
		case SDL_SCANCODE_SEMICOLON: return 0x27;
		case SDL_SCANCODE_APOSTROPHE: return 0x28;
		case SDL_SCANCODE_GRAVE: return 0x29;
		case SDL_SCANCODE_COMMA: return 0x33;
		case SDL_SCANCODE_PERIOD: return 0x34;
		case SDL_SCANCODE_SLASH: return 0x35;
		case SDL_SCANCODE_CAPSLOCK: return 0x3a;
		case SDL_SCANCODE_F11: return 0x57;
		case SDL_SCANCODE_F12: return 0x58;
		case SDL_SCANCODE_PRINTSCREEN: return 0x69;
		case SDL_SCANCODE_SCROLLLOCK: return 0x46;
		case SDL_SCANCODE_PAUSE: return 0x59;
		case SDL_SCANCODE_INSERT: return 0x5e;
		case SDL_SCANCODE_HOME: return 0x61;
		case SDL_SCANCODE_PAGEUP: return 0x63;
		case SDL_SCANCODE_DELETE: return 0x5f;
		case SDL_SCANCODE_END: return 0x62;
		case SDL_SCANCODE_PAGEDOWN: return 0x64;
		case SDL_SCANCODE_RIGHT: return 0x6c;
		case SDL_SCANCODE_LEFT: return 0x6b;
		case SDL_SCANCODE_DOWN: return 0x6a;
		case SDL_SCANCODE_UP: return 0x5a;
		case SDL_SCANCODE_NUMLOCKCLEAR: return 0x45;
		case SDL_SCANCODE_KP_DIVIDE: return 0x67;
		case SDL_SCANCODE_KP_MULTIPLY: return 0x37;
		case SDL_SCANCODE_KP_MINUS: return 0x4a;
		case SDL_SCANCODE_KP_PLUS: return 0x4e;
		case SDL_SCANCODE_KP_ENTER: return 0x68;
		case SDL_SCANCODE_KP_1: return 0x4f;
		case SDL_SCANCODE_KP_2: return 0x50;
		case SDL_SCANCODE_KP_3: return 0x51;
		case SDL_SCANCODE_KP_4: return 0x4b;
		case SDL_SCANCODE_KP_5: return 0x4c;
		case SDL_SCANCODE_KP_6: return 0x4d;
		case SDL_SCANCODE_KP_7: return 0x47;
		case SDL_SCANCODE_KP_8: return 0x48;
		case SDL_SCANCODE_KP_9: return 0x49;
		case SDL_SCANCODE_KP_0: return 0x52;
		case SDL_SCANCODE_KP_PERIOD: return 0x53;
		case SDL_SCANCODE_LCTRL: return 0x1d;
		case SDL_SCANCODE_LSHIFT: return 0x2a;
		case SDL_SCANCODE_LALT: return 0x38;
		case SDL_SCANCODE_RCTRL: return 0x66;
		case SDL_SCANCODE_RSHIFT: return 0x36;
		case SDL_SCANCODE_RALT: return 0x65;
		default: return 0;
	}
}

//---------------------------------------------------------------------------
// Platform thread
//---------------------------------------------------------------------------

static int platformthread(void *arg)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	static unsigned int rgba[1600 * 1200];

	(void)arg;
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "platform: SDL_Init failed: %s\n", SDL_GetError());
		running = -1;
		return 0;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	running = 1;

	while (running == 1)
	{
		SDL_Event ev;

		// Video mode change requested by the game thread.
		if (wantx && (wantx != curx || wanty != cury))
		{
			if (texture) SDL_DestroyTexture(texture);
			if (!window)
			{
				window = SDL_CreateWindow("Duke Nukem 3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					640, 480, SDL_WINDOW_RESIZABLE);
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
				if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
			}
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, wantx, wanty);
			// The original modes were shown on 4:3 monitors (320x200 has non-square pixels).
			SDL_RenderSetLogicalSize(renderer, 640, 480);
			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDL_LockMutex(lock);
			curx = wantx; cury = wanty;
			SDL_CondBroadcast(cond);
			SDL_UnlockMutex(lock);
		}

		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
				case SDL_QUIT:
					quitrequested = 1;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
					int sc = mactscancode(ev.key.keysym.scancode);
					if (ev.key.repeat) break;
					if (sc) KB_KeyEvent(sc, ev.type == SDL_KEYDOWN);
					if (ev.type == SDL_KEYDOWN) keyhit = 1;
					break;
				}
				case SDL_MOUSEMOTION:
					mousedx += ev.motion.xrel;
					mousedy += ev.motion.yrel;
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				{
					int bit = 0;
					if (ev.button.button == SDL_BUTTON_LEFT) bit = 1;
					else if (ev.button.button == SDL_BUTTON_RIGHT) bit = 2;
					else if (ev.button.button == SDL_BUTTON_MIDDLE) bit = 4;
					if (ev.type == SDL_MOUSEBUTTONDOWN) mousebuttons |= bit;
					else mousebuttons &= ~bit;
					break;
				}
				case SDL_WINDOWEVENT:
					if (ev.window.event == SDL_WINDOWEVENT_FOCUS_LOST) SDL_SetRelativeMouseMode(SDL_FALSE);
					if (ev.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) SDL_SetRelativeMouseMode(SDL_TRUE);
					break;
			}
		}

		if (framedirty && texture)
		{
			long i, n;
			unsigned int pal32[256];

			SDL_LockMutex(lock);
			for (i = 0; i < 256; i++)
			{
				const unsigned char *p = &palette[i * 4];
				pal32[i] = 0xff000000u | ((unsigned)(p[2] << 2) << 16) | ((unsigned)(p[1] << 2) << 8) | (unsigned)(p[0] << 2);
			}
			n = framex * framey;
			for (i = 0; i < n; i++) rgba[i] = pal32[frame[i]];
			framedirty = 0;
			SDL_UnlockMutex(lock);

			SDL_UpdateTexture(texture, NULL, rgba, (int)framex * 4);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);   // waits for vsync if available
		}
		else SDL_Delay(1);
	}

	if (texture) SDL_DestroyTexture(texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	return 0;
}

static void platform_shutdown(void)
{
	if (running == 1)
	{
		running = 0;
		SDL_WaitThread(thread, NULL);
	}
	SDL_Quit();
}

	// Starts the platform thread (on first use).
static void platform_start(void)
{
	if (thread) return;
	SDL_Init(0);
	lock = SDL_CreateMutex();
	cond = SDL_CreateCond();
	thread = SDL_CreateThread(platformthread, "platform", NULL);
	while (running == 0) SDL_Delay(1);
	atexit(platform_shutdown);
}

	// The window's close button ends the game.
static void checkquit(void)
{
	if (quitrequested) { quitrequested = 0; exit(0); }
}

//---------------------------------------------------------------------------
// Platform API (game thread)
//---------------------------------------------------------------------------

int platform_setvideomode(long xdim, long ydim)
{
	if (xdim * ydim > (long)sizeof(frame)) return -1;
	platform_start();
	if (running != 1) return -1;
	SDL_LockMutex(lock);
	wantx = xdim; wanty = ydim;
	while (curx != xdim || cury != ydim) SDL_CondWait(cond, lock);
	SDL_UnlockMutex(lock);
	return 0;
}

void platform_closevideo(void) { }

	// Writes frame as a binary PPM using the current palette (6-bit -> 8-bit).
static void dumpframe(const unsigned char *f, long xdim, long ydim, long pitch, const char *dir)
{
	char name[1024];
	FILE *fp;
	long x, y;

	snprintf(name, sizeof(name), "%s/frame%06ld.ppm", dir, framecount);
	if ((fp = fopen(name, "wb")) == NULL) return;
	fprintf(fp, "P6\n%ld %ld\n255\n", xdim, ydim);
	for (y = 0; y < ydim; y++)
		for (x = 0; x < xdim; x++)
		{
			const unsigned char *p = &palette[f[y * pitch + x] * 4];
			unsigned char rgb[3] = { (unsigned char)(p[2] << 2), (unsigned char)(p[1] << 2), (unsigned char)(p[0] << 2) };
			fwrite(rgb, 1, 3, fp);
		}
	fclose(fp);
}

void platform_present(const unsigned char *f, long xdim, long ydim, long pitch)
{
	const char *dir = getenv("DUKE_FRAMEDUMP");
	const char *every = getenv("DUKE_FRAMEDUMP_EVERY");
	long n = every ? atol(every) : 30, y;

	checkquit();
	if (dir && n > 0 && (framecount % n) == 0) dumpframe(f, xdim, ydim, pitch, dir);
	framecount++;

	if (running != 1) return;
	SDL_LockMutex(lock);
	for (y = 0; y < ydim; y++) memcpy(&frame[y * xdim], &f[y * pitch], xdim);
	framex = xdim; framey = ydim;
	framedirty = 1;
	SDL_UnlockMutex(lock);
}

void platform_setpalette(long start, long num, const unsigned char *pal)
{
	if (start < 0 || start + num > 256) return;
	if (lock) SDL_LockMutex(lock);
	memcpy(&palette[start * 4], pal, num * 4);
	framedirty = 1;
	if (lock) SDL_UnlockMutex(lock);
}

void platform_getpalette(long start, long num, unsigned char *pal)
{
	if (start < 0 || start + num > 256) return;
	memcpy(pal, &palette[start * 4], num * 4);
}

void platform_setvmode(long mode) { (void)mode; }

int platform_mousepresent(void) { return 1; }

void platform_readmousexy(short *dx, short *dy)
{
	int x = mousedx, y = mousedy;
	mousedx -= x; mousedy -= y;
	*dx = (short)x; *dy = (short)y;
	checkquit();
}

int platform_readmousebuttons(void) { return mousebuttons; }

void platform_outp(long port, long val) { (void)port; (void)val; }
long platform_inp(long port) { (void)port; return 0; }

unsigned long platform_ticks_us(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (unsigned long)((unsigned long long)ts.tv_sec * 1000000ull + ts.tv_nsec / 1000);
}

void platform_sleep_ms(unsigned ms) { SDL_Delay(ms); }

int platform_kbhit(void) { checkquit(); return keyhit; }

int platform_getch(void)
{
	while (!keyhit) { checkquit(); SDL_Delay(5); }
	keyhit = 0;
	return 13;
}
