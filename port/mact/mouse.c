// PORT: reimplementation of MACT386.LIB mouse functions (vidmsdos.c: int 33h).
#include "compat.h"
#include "types.h"
#include "mouse.h"
#include "platform.h"

boolean MOUSE_Init(void) { return platform_mousepresent() != 0; }
void    MOUSE_Shutdown(void) { }
void    MOUSE_ShowCursor(void) { }
void    MOUSE_HideCursor(void) { }

	// Bit 0 left, bit 1 right, bit 2 middle.
int32 MOUSE_GetButtons(void) { return platform_readmousebuttons(); }

void MOUSE_GetPosition(int32 *x, int32 *y) { *x = 0; *y = 0; }

	// Motion since the last call (int 33h function 0x0b).
void MOUSE_GetDelta(int32 *x, int32 *y)
{
	short dx, dy;
	platform_readmousexy(&dx, &dy);
	*x = dx; *y = dy;
}
