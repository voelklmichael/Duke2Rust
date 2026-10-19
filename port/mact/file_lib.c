// PORT: reimplementation of MACT386.LIB file_lib.c (no source was released).
#include <ctype.h>
#include "compat.h"
#include "types.h"
#include "util_lib.h"
#include "file_lib.h"

static int32 safeopen(const char *filename, int flags, const char *what)
{
	int fd = open(filename, flags | O_BINARY, 0644);
	if (fd < 0) Error("Error opening %s for %s", filename, what);
	return fd;
}

int32 SafeOpenWrite(const char *filename, int32 filetype)
	{ return safeopen(filename, O_WRONLY | O_CREAT | O_TRUNC, "writing"); }
int32 SafeOpenRead(const char *filename, int32 filetype)
	{ return safeopen(filename, O_RDONLY, "reading"); }
int32 SafeOpenAppend(const char *filename, int32 filetype)
	{ return safeopen(filename, O_WRONLY | O_CREAT | O_APPEND, "appending"); }

void SafeClose(int32 handle) { close(handle); }

boolean SafeFileExists(const char *filename) { return access(filename, F_OK) == 0; }

int32 SafeFileLength(int32 handle) { return filelength(handle); }

void SafeRead(int32 handle, void *buffer, int32 count)
{
	if (read(handle, buffer, count) != count) Error("File read failure reading %ld bytes", (long)count);
}

void SafeWrite(int32 handle, void *buffer, int32 count)
{
	if (write(handle, buffer, count) != count) Error("File write failure writing %ld bytes", (long)count);
}

int32 LoadFile(const char *filename, void **bufferptr)
{
	int32 handle = SafeOpenRead(filename, filetype_binary);
	int32 length = SafeFileLength(handle);
	void *buffer = SafeMalloc(length);
	SafeRead(handle, buffer, length);
	SafeClose(handle);
	*bufferptr = buffer;
	return length;
}

void SaveFile(const char *filename, void *bufferptr, int32 count)
{
	int32 handle = SafeOpenWrite(filename, filetype_binary);
	SafeWrite(handle, bufferptr, count);
	SafeClose(handle);
}

void GetPathFromEnvironment(char *fullname, const char *envname, const char *filename)
{
	const char *path = getenv(envname);
	if (path && path[0]) sprintf(fullname, "%s/%s", path, filename);
	else strcpy(fullname, filename);
}

void DefaultExtension(char *path, const char *extension)
{
	char *src = path + strlen(path) - 1;
	while (src >= path && *src != '/' && *src != '\\')
	{
		if (*src == '.') return;
		src--;
	}
	strcat(path, extension);
}

void DefaultPath(char *path, const char *basepath)
{
	char temp[128];
	if (path[0] == '/' || path[0] == '\\') return;
	strcpy(temp, path);
	strcpy(path, basepath);
	strcat(path, temp);
}

void ExtractFileBase(char *path, char *dest)
{
	char *src = path + strlen(path) - 1;
	int length = 0;
	while (src != path && src[-1] != '/' && src[-1] != '\\') src--;
	while (*src && *src != '.' && length < 8) { *dest++ = toupper((unsigned char)*src++); length++; }
	*dest = 0;
}

boolean GetExtension(char *filename, char *extension)
{
	char *p = strrchr(filename, '.');
	if (!p || strchr(p, '/') || strchr(p, '\\')) { extension[0] = 0; return false; }
	strcpy(extension, p + 1);
	return true;
}

void SetExtension(char *filename, const char *extension)
{
	char *p = strrchr(filename, '.');
	if (p && !strchr(p, '/') && !strchr(p, '\\')) *p = 0;
	strcat(filename, extension);
}
