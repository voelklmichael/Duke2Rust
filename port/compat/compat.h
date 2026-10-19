// DOS/Watcom compatibility layer for the Linux port.
// Provides the DOS-specific functions and keywords the original code uses.
#ifndef __COMPAT_H
#define __COMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
// unistd.h declares sync(), which clashes with Duke's global "sync" input array.
#define sync compat_unistd_sync
#include <unistd.h>
#undef sync
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// Watcom keywords that have no meaning in a flat 32-bit Linux process.
#define __far
#define far
#define __near
#define near
#define __interrupt
#define interrupt
#define __loadds
#define _loadds
#define cdecl
#define __cdecl

// Watcom C library functions.
#define stricmp  strcasecmp
#define strcmpi  strcasecmp
#define strnicmp strncasecmp
#define memicmp  strncasecmp
char *strupr(char *s);
char *strlwr(char *s);
char *ltoa(long value, char *buf, int radix);
char *itoa(int value, char *buf, int radix);
char *ultoa(unsigned long value, char *buf, int radix);

// Watcom's <stdlib.h> defines these as macros.
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

// Watcom's C library keeps the command line in these globals (set in main()).
extern int _argc;
extern char **_argv;

#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef O_TEXT
#define O_TEXT 0
#endif
#ifndef S_IREAD
#define S_IREAD  S_IRUSR
#endif
#ifndef S_IWRITE
#define S_IWRITE S_IWUSR
#endif

long filelength(int handle);
long tell(int handle);

// DOS file names are case-insensitive: these try the exact name first, then a
// case-insensitive match in the same directory.
int   compat_open(const char *path, int flags, ...);
FILE *compat_fopen(const char *path, const char *mode);
int   compat_access(const char *path, int mode);
#define open   compat_open
#define fopen  compat_fopen
#define access compat_access

#endif
