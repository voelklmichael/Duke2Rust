// PORT: reimplementation of MACT386.LIB scriplib.c (no source was released).
// Reads and writes INI-style script files (DUKE3D.CFG):
//   [Section]
//   Entry = value          ; value: number, 0x-hex, ~ (default), "string" or "a" "b"
// Comment and blank lines are kept so that saving preserves them.
#include <stdarg.h>
#include <ctype.h>
#include "compat.h"
#include "types.h"
#include "util_lib.h"
#include "file_lib.h"
#include "scriplib.h"

#define MAXSCRIPTFILES 20
#define MAXSTRING 80

typedef struct scriptline
{
	char *name;        // entry name, or NULL for a comment/blank line
	char *value;       // raw value (entry), or the whole line (comment)
	struct scriptline *next;
} scriptline;

typedef struct scriptsection
{
	char *name;
	scriptline *lines;
	struct scriptsection *next;
} scriptsection;

typedef struct { scriptsection *sections; } scriptfile;

static scriptfile *scriptfiles[MAXSCRIPTFILES];

static char *dupstr(const char *s) { char *d = SafeMalloc((int32)strlen(s) + 1); strcpy(d, s); return d; }

static scriptfile *SCRIPT_File(int32 handle)
{
	if (handle < 0 || handle >= MAXSCRIPTFILES || !scriptfiles[handle]) Error("SCRIPT: Invalid handle %ld", (long)handle);
	return scriptfiles[handle];
}

static scriptsection *SCRIPT_FindSection(scriptfile *f, const char *name)
{
	scriptsection *s;
	for (s = f->sections; s; s = s->next) if (!strcasecmp(s->name, name)) return s;
	return NULL;
}

static scriptsection *SCRIPT_AddSection(scriptfile *f, const char *name)
{
	scriptsection *s = SCRIPT_FindSection(f, name), **p;
	if (s) return s;
	s = SafeMalloc(sizeof(*s));
	s->name = dupstr(name);
	s->lines = NULL;
	s->next = NULL;
	for (p = &f->sections; *p; p = &(*p)->next) ;
	*p = s;
	return s;
}

static scriptline *SCRIPT_FindEntry(scriptsection *s, const char *name)
{
	scriptline *l;
	if (!s) return NULL;
	for (l = s->lines; l; l = l->next) if (l->name && !strcasecmp(l->name, name)) return l;
	return NULL;
}

static void SCRIPT_AppendLine(scriptsection *s, const char *name, const char *value)
{
	scriptline *l = SafeMalloc(sizeof(*l)), **p;
	l->name = name ? dupstr(name) : NULL;
	l->value = dupstr(value);
	l->next = NULL;
	for (p = &s->lines; *p; p = &(*p)->next) ;
	*p = l;
}

	// Sets (or adds) an entry's raw value.
static void SCRIPT_AddEntry(int32 handle, const char *section, const char *entry, const char *value)
{
	scriptsection *s = SCRIPT_AddSection(SCRIPT_File(handle), section);
	scriptline *l = SCRIPT_FindEntry(s, entry);
	if (l) { free(l->value); l->value = dupstr(value); }
	else SCRIPT_AppendLine(s, entry, value);
}

int32 SCRIPT_Init(char *name)
{
	int32 i;
	(void)name;
	for (i = 0; i < MAXSCRIPTFILES; i++)
		if (!scriptfiles[i])
		{
			scriptfiles[i] = SafeMalloc(sizeof(scriptfile));
			scriptfiles[i]->sections = NULL;
			return i;
		}
	return -1;
}

void SCRIPT_Free(int32 scripthandle)
{
	scriptfile *f = SCRIPT_File(scripthandle);
	scriptsection *s, *ns;
	scriptline *l, *nl;
	for (s = f->sections; s; s = ns)
	{
		for (l = s->lines; l; l = nl) { nl = l->next; free(l->name); free(l->value); free(l); }
		ns = s->next; free(s->name); free(s);
	}
	free(f);
	scriptfiles[scripthandle] = NULL;
}

static char *trim(char *s)
{
	char *e;
	while (*s && isspace((unsigned char)*s)) s++;
	e = s + strlen(s);
	while (e > s && isspace((unsigned char)e[-1])) *--e = 0;
	return s;
}

int32 SCRIPT_Parse(char *data, int32 length, char *name)
{
	int32 handle = SCRIPT_Init(name);
	scriptfile *f;
	scriptsection *cur;
	int32 pos = 0;

	if (handle < 0) return -1;
	f = scriptfiles[handle];
	cur = SCRIPT_AddSection(f, "");   // lines before the first section

	while (pos < length)
	{
		char line[1024], *t, *eq;
		int32 n = 0;
		while (pos < length && data[pos] != '\n' && data[pos] != '\r')
		{
			if (n < (int32)sizeof(line) - 1) line[n++] = data[pos];
			pos++;
		}
		if (pos < length && data[pos] == '\r') pos++;
		if (pos < length && data[pos] == '\n') pos++;
		line[n] = 0;

		t = trim(line);
		if (t[0] == '[' && strchr(t, ']'))
		{
			*strchr(t, ']') = 0;
			cur = SCRIPT_AddSection(f, t + 1);
		}
		else if (t[0] != ';' && t[0] != 0 && (eq = strchr(t, '=')) != NULL)
		{
			*eq = 0;
			SCRIPT_AppendLine(cur, trim(t), trim(eq + 1));
		}
		else SCRIPT_AppendLine(cur, NULL, t);
	}
	return handle;
}

int32 SCRIPT_Load(char *filename)
{
	void *data;
	int32 length, handle;

	if (!SafeFileExists(filename)) return -1;
	length = LoadFile(filename, &data);
	handle = SCRIPT_Parse((char *)data, length, filename);
	free(data);
	return handle;
}

void SCRIPT_Save(int32 scripthandle, char *filename)
{
	scriptfile *f = SCRIPT_File(scripthandle);
	scriptsection *s;
	scriptline *l;
	FILE *fp = fopen(filename, "wb");

	if (!fp) Error("SCRIPT_Save: cannot write %s", filename);
	for (s = f->sections; s; s = s->next)
	{
		if (s->name[0]) fprintf(fp, "[%s]\r\n", s->name);
		for (l = s->lines; l; l = l->next)
		{
			if (l->name) fprintf(fp, "%s = %s\r\n", l->name, l->value);
			else fprintf(fp, "%s\r\n", l->value);
		}
	}
	fclose(fp);
}

int32 SCRIPT_NumberSections(int32 scripthandle)
{
	scriptsection *s;
	int32 n = 0;
	for (s = SCRIPT_File(scripthandle)->sections; s; s = s->next) if (s->name[0]) n++;
	return n;
}

char *SCRIPT_Section(int32 scripthandle, int32 which)
{
	scriptsection *s;
	for (s = SCRIPT_File(scripthandle)->sections; s; s = s->next)
		if (s->name[0] && which-- == 0) return s->name;
	return NULL;
}

int32 SCRIPT_NumberEntries(int32 scripthandle, char *sectionname)
{
	scriptsection *s = SCRIPT_FindSection(SCRIPT_File(scripthandle), sectionname);
	scriptline *l;
	int32 n = 0;
	if (!s) return 0;
	for (l = s->lines; l; l = l->next) if (l->name) n++;
	return n;
}

char *SCRIPT_Entry(int32 scripthandle, char *sectionname, int32 which)
{
	scriptsection *s = SCRIPT_FindSection(SCRIPT_File(scripthandle), sectionname);
	scriptline *l;
	if (!s) return NULL;
	for (l = s->lines; l; l = l->next) if (l->name && which-- == 0) return l->name;
	return NULL;
}

char *SCRIPT_GetRaw(int32 scripthandle, char *sectionname, char *entryname)
{
	scriptline *l = SCRIPT_FindEntry(SCRIPT_FindSection(SCRIPT_File(scripthandle), sectionname), entryname);
	return l ? l->value : NULL;
}

	// Copies the quoted string starting at *p (which must be '"') to dest
	// (max 80 characters) and returns the position after the closing quote.
static char *SCRIPT_CopyQuoted(char *p, char *dest, char *entryname)
{
	char *start;
	if (*p != '"') Error("SCRIPT_GetString: Expected a string starting with %c for %s", '"', entryname);
	start = ++p;
	while (*p && *p != '"')
	{
		if (p - start >= MAXSTRING) Error("SCRIPT_GetString: String too long for %s", entryname);
		p++;
	}
	memcpy(dest, start, p - start);
	dest[p - start] = 0;
	return *p ? p + 1 : p;
}

	// Leaves dest unchanged if the entry does not exist.
void SCRIPT_GetString(int32 scripthandle, char *sectionname, char *entryname, char *dest)
{
	char *raw = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
	if (raw) SCRIPT_CopyQuoted(raw, dest, entryname);
}

void SCRIPT_GetDoubleString(int32 scripthandle, char *sectionname, char *entryname, char *dest1, char *dest2)
{
	char *raw = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
	if (!raw) return;
	raw = SCRIPT_CopyQuoted(raw, dest1, entryname);
	while (*raw && *raw != '"') raw++;
	if (*raw) SCRIPT_CopyQuoted(raw, dest2, entryname);
}

	// Leaves number unchanged if the entry does not exist or is "~" (default).
	// Returns true only for "~".
boolean SCRIPT_GetNumber(int32 scripthandle, char *sectionname, char *entryname, int32 *number)
{
	char *raw = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
	long v;
	if (!raw) return false;
	if (raw[0] == '~') return true;
	if (raw[0] == '0' && raw[1] == 'x') { if (sscanf(raw + 2, "%lx", (unsigned long *)&v) == 1) *number = v; }
	else if (sscanf(raw, "%ld", &v) == 1) *number = v;
	return false;
}

void SCRIPT_GetBoolean(int32 scripthandle, char *sectionname, char *entryname, boolean *b)
{
	int32 n = *b;
	SCRIPT_GetNumber(scripthandle, sectionname, entryname, &n);
	*b = (n != 0);
}

void SCRIPT_GetDouble(int32 scripthandle, char *sectionname, char *entryname, double *number)
{
	char *raw = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
	if (raw && raw[0] != '~') sscanf(raw, "%lf", number);
}

void SCRIPT_PutComment(int32 scripthandle, char *sectionname, char *comment)
{
	char buf[256];
	snprintf(buf, sizeof(buf), "; %s", comment);
	SCRIPT_AppendLine(SCRIPT_AddSection(SCRIPT_File(scripthandle), sectionname), NULL, buf);
}

void SCRIPT_PutEOL(int32 scripthandle, char *sectionname)
{
	SCRIPT_AppendLine(SCRIPT_AddSection(SCRIPT_File(scripthandle), sectionname), NULL, "");
}

void SCRIPT_PutMultiComment(int32 scripthandle, char *sectionname, char *comment, ...)
{
	va_list ap;
	char *c;
	va_start(ap, comment);
	for (c = comment; c; c = va_arg(ap, char *)) SCRIPT_PutComment(scripthandle, sectionname, c);
	va_end(ap);
}

void SCRIPT_PutSection(int32 scripthandle, char *sectionname)
{
	SCRIPT_AddSection(SCRIPT_File(scripthandle), sectionname);
}

void SCRIPT_PutRaw(int32 scripthandle, char *sectionname, char *entryname, char *raw)
{
	SCRIPT_AddEntry(scripthandle, sectionname, entryname, raw);
}

void SCRIPT_PutString(int32 scripthandle, char *sectionname, char *entryname, char *string)
{
	char buf[MAXSTRING + 3];
	snprintf(buf, sizeof(buf), "\"%s\"", string);
	SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutDoubleString(int32 scripthandle, char *sectionname, char *entryname, char *string1, char *string2)
{
	char buf[2 * MAXSTRING + 6];
	snprintf(buf, sizeof(buf), "\"%s\" \"%s\"", string1, string2);
	SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutNumber(int32 scripthandle, char *sectionname, char *entryname, int32 number, boolean hexadecimal, boolean defaultvalue)
{
	char buf[32];
	if (defaultvalue) strcpy(buf, "~");
	else if (hexadecimal) sprintf(buf, "0x%lX", (unsigned long)number);
	else sprintf(buf, "%ld", (long)number);
	SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutBoolean(int32 scripthandle, char *sectionname, char *entryname, boolean b)
{
	SCRIPT_AddEntry(scripthandle, sectionname, entryname, b ? "1" : "0");
}

void SCRIPT_PutDouble(int32 scripthandle, char *sectionname, char *entryname, double number, boolean defaultvalue)
{
	char buf[64];
	if (defaultvalue) strcpy(buf, "~");
	else sprintf(buf, "%g", number);
	SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}
