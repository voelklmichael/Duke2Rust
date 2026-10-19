// PORT: reimplementation of MACT386.LIB util_lib.c / mathutil.c (no source was
// released). Behaviour follows the library's disassembly where it matters.
#include <stdarg.h>
#include "compat.h"
#include "types.h"
#include "util_lib.h"

#define MAXSHUTDOWNFUNCS 32
static void (*shutdownfuncs[MAXSHUTDOWNFUNCS])(void);
static int numshutdownfuncs = 0;

void RegisterShutdownFunction(void (*shutdown)(void))
{
	if (numshutdownfuncs < MAXSHUTDOWNFUNCS) shutdownfuncs[numshutdownfuncs++] = shutdown;
}

	// Runs the registered shutdown functions (last registered first), prints the message, exits.
void Error(char *error, ...)
{
	va_list ap;
	static int inerror = 0;

	if (!inerror)
	{
		inerror = 1;
		while (numshutdownfuncs > 0) shutdownfuncs[--numshutdownfuncs]();
	}
	va_start(ap, error);
	vfprintf(stderr, error, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

	// Returns the index (1..argc-1) of the command line parameter matching check
	// (case-insensitive, leading '-', '/' or '\' ignored), or 0.
char CheckParm(char *check)
{
	char buf[128], *tok;
	int i;

	for (i = 1; i < _argc; i++)
	{
		snprintf(buf, sizeof(buf), "%s", _argv[i]);
		tok = strtok(buf, "\\-/");
		if (tok && !strcasecmp(check, tok)) return (char)i;
	}
	return 0;
}

void *SafeMalloc(int32 size)
{
	void *p = malloc(size > 0 ? size : 1);
	if (!p) Error("SafeMalloc failure for %ld bytes", (long)size);
	return p;
}

int32 SafeMallocSize(void *ptr) { (void)ptr; return 0; }

void SafeFree(void *ptr)
{
	if (!ptr) Error("SafeFree: Tried to deallocate NULL pointer.");
	free(ptr);
}

void SafeRealloc(void **ptr, int32 newsize)
{
	void *p = realloc(*ptr, newsize > 0 ? newsize : 1);
	if (!p) Error("SafeRealloc failure for %ld bytes", (long)newsize);
	*ptr = p;
}

int32 ParseHex(char *hex) { return (int32)strtoul(hex, NULL, 16); }

int32 ParseNum(char *str)
{
	if (str[0] == '$') return ParseHex(str + 1);
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) return ParseHex(str + 2);
	return (int32)atol(str);
}

	// The data files are little-endian, like x86.
int16 IntelShort(int16 l) { return l; }
int32 IntelLong(int32 l) { return l; }
int16 MotoShort(int16 l) { return (int16)(((l & 0xff) << 8) | ((l >> 8) & 0xff)); }
int32 MotoLong(int32 l)
{
	unsigned long u = (unsigned long)l;
	return (int32)((u >> 24) | ((u >> 8) & 0xff00) | ((u << 8) & 0xff0000) | (u << 24));
}

	// Approximate distances (mathutil.c), exactly as in the original library.
int32 FindDistance2D(int32 dx, int32 dy)
{
	int32 x = labs(dx), y = labs(dy), t;

	if (y == 0) return x;
	if (x == 0) return y;
	if (x < y) { t = x; x = y; y = t; }
	t = y + (y >> 1);
	return x - (x >> 5) - (x >> 7) + (t >> 2) + (t >> 6);
}

int32 FindDistance3D(int32 dx, int32 dy, int32 dz)
{
	int32 x = labs(dx), y = labs(dy), z = labs(dz), t;

	if (x < y) { t = x; x = y; y = t; }
	if (x < z) { t = x; x = z; z = t; }
	t = y + z;
	return x - (x >> 4) + (t >> 2) + (t >> 3);
}

	// Free heap reported to the game's memory check (DOS: largest free block).
int32 Z_AvailHeap(void) { return 32 * 1024 * 1024; }
