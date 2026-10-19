// PORT: reimplementation of MACT386.LIB keyboard.c (no source was released),
// following its disassembly. On DOS, KB_KeyEvent was called from the keyboard
// interrupt; here the platform layer calls it from its input thread.
#include "compat.h"
#include "types.h"
#include "keyboard.h"
#include "kb_tables.h"

volatile byte KB_KeyDown[MAXKEYBOARDSCAN];
volatile kb_scancode KB_LastScan;

static volatile byte KeyboardQueue[256];
static volatile int32 Keyhead = 0, Keytail = 0;
static int32 KeyPadActive = 0;
static int32 KeyboardStarted = 0;

static void queuechar(char ch)
{
	int32 next = (Keytail + 1) & 255;
	if (next != Keyhead) { KeyboardQueue[Keytail] = ch; Keytail = next; }
}

	// Sets the key state and queues its ASCII value. Keys without one (and
	// keypad keys unless the keypad is active) queue a 0 followed by the scancode.
void KB_KeyEvent(int scancode, boolean keypressed)
{
	char ch;

	if (!keypressed) { KB_KeyDown[scancode] = 0; return; }

	KB_KeyDown[scancode] = 1;
	KB_LastScan = scancode;

	if (!KeyPadActive && ((scancode >= 0x47 && scancode <= 0x53) ||
		 scancode == 0x67 || scancode == 0x68 || scancode == 0x37))
		ch = 0;
	else if (KB_KeyDown[sc_RightShift] || KB_KeyDown[sc_LeftShift])
		ch = ShiftNames[scancode];
	else
		ch = ASCIINames[scancode];

	queuechar(ch);
	if (ch == 0) queuechar((char)scancode);
}

boolean KB_KeyWaiting(void) { return Keyhead != Keytail; }

char KB_Getch(void)
{
	char ch;
	while (Keyhead == Keytail) ;   // the input thread fills the queue
	ch = KeyboardQueue[Keyhead];
	Keyhead = (Keyhead + 1) & 255;
	return ch;
}

void KB_Addch(char ch) { queuechar(ch); }

void KB_FlushKeyboardQueue(void) { Keytail = Keyhead; }

void KB_ClearKeysDown(void)
{
	KB_LastScan = 0;
	memset((void *)KB_KeyDown, 0, sizeof(KB_KeyDown));
}

char *KB_ScanCodeToString(kb_scancode scancode)
{
	if (scancode >= sizeof(ScanStrings) / sizeof(ScanStrings[0])) return (char *)ScanStrings[0];
	return (char *)ScanStrings[scancode];
}

	// Returns the scancode whose name matches (case-insensitive), or 0.
kb_scancode KB_StringToScanCode(char *string)
{
	unsigned i;
	for (i = 0; i < sizeof(ScanStrings) / sizeof(ScanStrings[0]); i++)
		if (!strcasecmp(ScanStrings[i], string)) return (kb_scancode)i;
	return 0;
}

void KB_TurnKeypadOn(void) { KeyPadActive = 1; }
void KB_TurnKeypadOff(void) { KeyPadActive = 0; }
boolean KB_KeypadActive(void) { return KeyPadActive; }

void KB_Startup(void)
{
	if (KeyboardStarted) return;
	KeyboardStarted = 1;
	KB_FlushKeyboardQueue();
	KB_ClearKeysDown();
	printf("KB_Startup: Keyboard Started\n");
}

void KB_Shutdown(void)
{
	if (!KeyboardStarted) return;
	KeyboardStarted = 0;
	KB_FlushKeyboardQueue();
	KB_ClearKeysDown();
}
