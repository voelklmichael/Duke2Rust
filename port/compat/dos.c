// Implementation of the DOS/Watcom compatibility layer (see compat.h, dos.h).
#define _GNU_SOURCE   // FNM_CASEFOLD
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include "compat.h"
#include "dos.h"
#include "conio.h"
#include "bios.h"
#include "platform.h"

// The macros in compat.h redirect these names; here we need the real ones.
#undef open
#undef fopen
#undef access

long _argc;
char **_argv;

char *strupr(char *s) { char *p; for (p = s; *p; p++) *p = toupper((unsigned char)*p); return s; }
char *strlwr(char *s) { char *p; for (p = s; *p; p++) *p = tolower((unsigned char)*p); return s; }

char *ultoa(unsigned long value, char *buf, int radix)
{
	char tmp[33];
	int i = 0, j = 0;
	do { int d = value % radix; tmp[i++] = (char)(d < 10 ? '0' + d : 'a' + d - 10); value /= radix; } while (value);
	while (i) buf[j++] = tmp[--i];
	buf[j] = 0;
	return buf;
}

char *ltoa(long value, char *buf, int radix)
{
	if (radix == 10 && value < 0) { buf[0] = '-'; ultoa(0ul - (unsigned long)value, buf + 1, radix); return buf; }
	return ultoa((unsigned long)value, buf, radix);
}

char *itoa(int value, char *buf, int radix) { return ltoa(value, buf, radix); }

long filelength(int handle)
{
	struct stat st;
	if (fstat(handle, &st) != 0) return -1;
	return (long)st.st_size;
}

long tell(int handle) { return (long)lseek(handle, 0, SEEK_CUR); }

// Finds a case-insensitive match for path. Returns 1 and fills out on success.
static int findcase(const char *path, char *out, size_t outsize)
{
	char dir[1024];
	const char *base = strrchr(path, '/');
	DIR *d;
	struct dirent *e;

	if (base) { size_t n = (size_t)(base - path); if (n >= sizeof(dir)) return 0; memcpy(dir, path, n); dir[n] = 0; base++; }
	else { strcpy(dir, "."); base = path; }

	if ((d = opendir(dir)) == NULL) return 0;
	while ((e = readdir(d)) != NULL)
	{
		if (strcasecmp(e->d_name, base) == 0)
		{
			if (strcmp(dir, ".") == 0 && !strchr(path, '/')) snprintf(out, outsize, "%s", e->d_name);
			else snprintf(out, outsize, "%s/%s", dir, e->d_name);
			closedir(d);
			return 1;
		}
	}
	closedir(d);
	return 0;
}

// DOS paths may use backslashes.
static const char *fixslashes(const char *path, char *buf, size_t bufsize)
{
	size_t i;
	if (!strchr(path, '\\')) return path;
	for (i = 0; path[i] && i + 1 < bufsize; i++) buf[i] = (path[i] == '\\') ? '/' : path[i];
	buf[i] = 0;
	return buf;
}

int compat_open(const char *path, int flags, ...)
{
	char fixed[1024], found[1024];
	int mode = 0, fd;
	if (flags & O_CREAT) { va_list ap; va_start(ap, flags); mode = va_arg(ap, int); va_end(ap); }
	if (mode == 0) mode = 0644;
	path = fixslashes(path, fixed, sizeof(fixed));
	fd = open(path, flags, mode);
	if (fd < 0 && errno == ENOENT && findcase(path, found, sizeof(found)))
		fd = open(found, flags & ~(O_CREAT | O_EXCL), mode);
	return fd;
}

FILE *compat_fopen(const char *path, const char *mode)
{
	char fixed[1024], found[1024];
	FILE *f;
	path = fixslashes(path, fixed, sizeof(fixed));
	f = fopen(path, mode);
	if (f == NULL && errno == ENOENT && findcase(path, found, sizeof(found)))
		f = fopen(found, mode);
	return f;
}

int compat_access(const char *path, int mode)
{
	char fixed[1024], found[1024];
	path = fixslashes(path, fixed, sizeof(fixed));
	if (access(path, mode) == 0) return 0;
	if (findcase(path, found, sizeof(found))) return access(found, mode);
	return -1;
}

// _dos_findfirst/_dos_findnext: case-insensitive wildcard search in one directory.
struct findstate { DIR *dir; char dirname[1024]; char pattern[256]; unsigned attrib; };

static unsigned findmatch(struct find_t *f)
{
	struct findstate *s = (struct findstate *)f->compat_state;
	struct dirent *e;
	while ((e = readdir(s->dir)) != NULL)
	{
		char full[1400];
		struct stat st;
		int isdir;
		if (strlen(e->d_name) > 12) continue;   // not a DOS 8.3 name
		if (fnmatch(s->pattern, e->d_name, FNM_CASEFOLD) != 0) continue;
		snprintf(full, sizeof(full), "%s/%s", s->dirname, e->d_name);
		if (stat(full, &st) != 0) continue;
		isdir = S_ISDIR(st.st_mode);
		// DOS semantics: normal files always match, directories only with _A_SUBDIR.
		if (isdir && !(s->attrib & _A_SUBDIR)) continue;
		memset(f, 0, offsetof(struct find_t, compat_state));
		f->attrib = isdir ? _A_SUBDIR : _A_NORMAL;
		f->size = (unsigned long)st.st_size;
		strcpy(f->name, e->d_name);
		strupr(f->name);
		return 0;
	}
	return 1;
}

unsigned _dos_findfirst(const char *pattern, unsigned attrib, struct find_t *f)
{
	char fixed[1024];
	struct findstate *s;
	const char *base;
	char *p;

	pattern = fixslashes(pattern, fixed, sizeof(fixed));
	s = (struct findstate *)calloc(1, sizeof(*s));
	base = strrchr(pattern, '/');
	if (base) { snprintf(s->dirname, sizeof(s->dirname), "%.*s", (int)(base - pattern), pattern); base++; }
	else { strcpy(s->dirname, "."); base = pattern; }
	snprintf(s->pattern, sizeof(s->pattern), "%s", base);
	// DOS "*.*" also matches names without an extension.
	if ((p = strstr(s->pattern, "*.*")) != NULL && p[3] == 0) strcpy(p, "*");
	s->attrib = attrib;
	if ((s->dir = opendir(s->dirname)) == NULL) { free(s); f->compat_state = NULL; return 1; }
	f->compat_state = s;
	if (findmatch(f) == 0) return 0;
	_dos_findclose(f);
	return 1;
}

unsigned _dos_findnext(struct find_t *f)
{
	if (f->compat_state == NULL) return 1;
	if (findmatch(f) == 0) return 0;
	_dos_findclose(f);
	return 1;
}

unsigned _dos_findclose(struct find_t *f)
{
	struct findstate *s = (struct findstate *)f->compat_state;
	if (s) { closedir(s->dir); free(s); f->compat_state = NULL; }
	return 0;
}

void _disable(void) { }
void _enable(void) { }
void delay(unsigned ms) { platform_sleep_ms(ms); }

int int386(int intno, union REGS *in, union REGS *out)
{
	fprintf(stderr, "int386(0x%x, ax=0x%x) ignored\n", intno, in->w.ax);
	if (out != in) *out = *in;
	out->x.cflag = 1;
	return 0;
}

int int386x(int intno, union REGS *in, union REGS *out, struct SREGS *s)
{
	(void)s;
	return int386(intno, in, out);
}

void segread(struct SREGS *s) { memset(s, 0, sizeof(*s)); }

int getch(void) { return platform_getch(); }
int kbhit(void) { return platform_kbhit(); }
int inp(unsigned port) { return (int)platform_inp(port); }
int outp(unsigned port, int value) { platform_outp(port, value); return value; }

unsigned _bios_timeofday(unsigned service, long *timeval)
{
	(void)service;
	*timeval = (long)((unsigned long long)platform_ticks_us() * 182 / 10000000);
	return 0;
}

unsigned _bios_keybrd(unsigned service)
{
	if (service == 1) return platform_kbhit() ? 1 : 0;
	return (unsigned)platform_getch();
}
