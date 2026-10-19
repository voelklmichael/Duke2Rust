// PORT: reimplementation of MACT386.LIB control.c (no source was released),
// following its disassembly. Joystick and "external" controllers (the DOS
// game-port and CyberMan/VFX1 drivers) are not supported.
#include "compat.h"
#include "types.h"
#include "keyboard.h"
#include "mouse.h"
#include "control.h"
#include "util_lib.h"

#define MAXFUNCTIONS 64
#define MAXDEVICEBUTTONS 32
#define MAXAXES 6
#define UNMAPPED 0x7f

typedef struct
{
	byte active;      // current state of the function
	byte used;        // defined with CONTROL_DefineFlag
	byte toggle;      // toggles on each press instead of following the key
	byte buttonheld;  // toggle: input still held since the last toggle
	int32 cleared;    // CONTROL_ClearButton: ignore until released
} controlflags;

typedef struct { int32 amount; int32 digital; } controlaxistype;
typedef struct { byte analogmap; byte minmap; byte maxmap; byte extra; } controlaxismaptype;
typedef struct { byte singleclicked; byte doubleclicked; word extra; } controlbuttontype;

boolean CONTROL_RudderEnabled = 0;
boolean CONTROL_MousePresent = 0;
boolean CONTROL_JoysPresent[MaxJoys];
boolean CONTROL_MouseEnabled = 0;
boolean CONTROL_JoystickEnabled = 0;
byte    CONTROL_JoystickPort = 0;
uint32  CONTROL_ButtonState1 = 0;
uint32  CONTROL_ButtonHeldState1 = 0;
uint32  CONTROL_ButtonState2 = 0;
uint32  CONTROL_ButtonHeldState2 = 0;

static int32 CONTROL_MouseSensitivity = 0x8000;
static int32 CONTROL_NumButtons = MAXDEVICEBUTTONS;
static int32 CONTROL_NumAxes = MAXAXES;

static controlaxistype CONTROL_Axes[MAXAXES], CONTROL_LastAxes[MAXAXES];
static int32 CONTROL_AxesScale[MAXAXES];
static controlaxismaptype CONTROL_AxesMap[MAXAXES];
static controlbuttontype CONTROL_DeviceButtonMapping[MAXDEVICEBUTTONS];
static int32 CONTROL_DeviceButtonState[MAXDEVICEBUTTONS];
static int32 CONTROL_ButtonClickedTime[MAXDEVICEBUTTONS];
static int32 CONTROL_ButtonClickedState[MAXDEVICEBUTTONS];
static int32 CONTROL_ButtonClicked[MAXDEVICEBUTTONS];
static byte  CONTROL_ButtonClickedCount[MAXDEVICEBUTTONS];
static controlflags CONTROL_Flags[MAXFUNCTIONS];
static byte CONTROL_KeyMapping[MAXFUNCTIONS][2];

static int32 (*GetTime)(void);
static int32 CONTROL_Started = 0;
static int32 ticrate;
static int32 CONTROL_DoubleClickSpeed;

static volatile int32 controltime = 0;
static int32 CONTROL_GetTime(void) { return controltime++; }

static void CONTROL_CheckRange(int32 which)
{
	if ((uint32)which >= MAXFUNCTIONS) Error("CONTROL_CheckRange: Index %ld out of valid range for %d control flags.", (long)which, MAXFUNCTIONS);
}

	// Mouse motion: the smaller axis is divided by 3, then both are scaled.
static void CONTROL_GetMouseDelta(int32 *x, int32 *y)
{
	MOUSE_GetDelta(x, y);
	if (labs(*x) > labs(*y)) *y /= 3;
	else *x /= 3;
	*x <<= 5;
	*y = *y * 96;
	*x = (int32)(((long long)*x * CONTROL_MouseSensitivity) >> 15);
}

int32 CONTROL_GetMouseSensitivity(void) { return CONTROL_MouseSensitivity - 0x1000; }
void CONTROL_SetMouseSensitivity(int32 newsensitivity) { CONTROL_MouseSensitivity = newsensitivity + 0x1000; }

static void CONTROL_SetFlag(int32 which, boolean active)
{
	controlflags *f;

	CONTROL_CheckRange(which);
	f = &CONTROL_Flags[which];
	if (!f->toggle) { f->active = (byte)active; return; }
	if (active)
	{
		if (!f->buttonheld) { f->active = (f->active == 0); f->buttonheld = 1; }
	}
	else f->buttonheld = 0;
}

static boolean CONTROL_KeyboardFunctionPressed(int32 which)
{
	CONTROL_CheckRange(which);
	if (!CONTROL_Flags[which].used) return 0;
	return KB_KeyDown[CONTROL_KeyMapping[which][0]] != 0 || KB_KeyDown[CONTROL_KeyMapping[which][1]] != 0;
}

void CONTROL_DefineFlag(int32 which, boolean toggle)
{
	controlflags *f;

	CONTROL_CheckRange(which);
	f = &CONTROL_Flags[which];
	f->toggle = (byte)toggle;
	f->used = 1;
	f->active = 0;
	f->buttonheld = 0;
	f->cleared = 0;
}

boolean CONTROL_FlagActive(int32 which)
{
	CONTROL_CheckRange(which);
	return CONTROL_Flags[which].used;
}

void CONTROL_MapKey(int32 which, kb_scancode key1, kb_scancode key2)
{
	CONTROL_CheckRange(which);
	CONTROL_KeyMapping[which][0] = key1;
	CONTROL_KeyMapping[which][1] = key2;
}

void CONTROL_MapButton(int32 whichfunction, int32 whichbutton, boolean doubleclicked)
{
	CONTROL_CheckRange(whichfunction);
	if (whichbutton < 0 || whichbutton >= CONTROL_NumButtons)
		Error("CONTROL_MapButton: button %ld out of valid range for %ld buttons.", (long)whichbutton, (long)CONTROL_NumButtons);
	if (doubleclicked) CONTROL_DeviceButtonMapping[whichbutton].doubleclicked = (byte)whichfunction;
	else CONTROL_DeviceButtonMapping[whichbutton].singleclicked = (byte)whichfunction;
}

void CONTROL_MapAnalogAxis(int32 whichaxis, int32 whichanalog)
{
	if (whichaxis < 0 || whichaxis >= MAXAXES) Error("CONTROL_MapAnalogAxis: axis %ld out of valid range for %d axes.", (long)whichaxis, MAXAXES);
	if (whichanalog < 0 || whichanalog >= analog_maxtype) Error("CONTROL_MapAnalogAxis: analog function %ld out of valid range for %d analog functions.", (long)whichanalog, analog_maxtype);
	CONTROL_AxesMap[whichaxis].analogmap = (byte)whichanalog;
}

void CONTROL_SetAnalogAxisScale(int32 whichaxis, int32 axisscale)
{
	if (whichaxis < 0 || whichaxis >= MAXAXES) Error("CONTROL_SetAnalogAxisScale: axis %ld out of valid range for %d axes.", (long)whichaxis, MAXAXES);
	CONTROL_AxesScale[whichaxis] = axisscale;
}

	// up/right map to the "min" (negative) digital direction, down/left to "max".
void CONTROL_MapDigitalAxis(int32 whichaxis, int32 whichfunction, int32 direction)
{
	CONTROL_CheckRange(whichfunction);
	if (whichaxis < 0 || whichaxis >= MAXAXES) Error("CONTROL_MapDigitalAxis: axis %ld out of valid range for %d axes.", (long)whichaxis, MAXAXES);
	switch (direction)
	{
		case axis_up:
		case axis_right: CONTROL_AxesMap[whichaxis].maxmap = (byte)whichfunction; break;
		case axis_down:
		case axis_left: CONTROL_AxesMap[whichaxis].minmap = (byte)whichfunction; break;
	}
}

void CONTROL_ClearAssignments(void)
{
	int32 i;

	memset(CONTROL_DeviceButtonMapping, UNMAPPED, sizeof(CONTROL_DeviceButtonMapping));
	memset(CONTROL_KeyMapping, UNMAPPED, sizeof(CONTROL_KeyMapping));
	memset(CONTROL_Axes, 0, sizeof(CONTROL_Axes));
	for (i = 0; i < MAXAXES; i++) CONTROL_AxesScale[i] = 0x10000;
	memset(CONTROL_LastAxes, 0, sizeof(CONTROL_LastAxes));
	memset(CONTROL_AxesMap, UNMAPPED, sizeof(CONTROL_AxesMap));
}

	// Button state and single/double click detection.
static void CONTROL_GetDeviceButtons(void)
{
	int32 t = GetTime(), buttons = 0, i;

	if (CONTROL_MouseEnabled) buttons = MOUSE_GetButtons() & 255;

	for (i = 0; i < CONTROL_NumButtons; i++)
	{
		int32 state = (buttons >> i) & 1;
		CONTROL_DeviceButtonState[i] = state;
		CONTROL_ButtonClickedState[i] = 0;
		if (state)
		{
			if (!CONTROL_ButtonClicked[i])
			{
				CONTROL_ButtonClicked[i] = 1;
				if (CONTROL_ButtonClickedCount[i] == 0 || t >= CONTROL_ButtonClickedTime[i])
				{
					CONTROL_ButtonClickedTime[i] = t + CONTROL_DoubleClickSpeed;
					CONTROL_ButtonClickedCount[i] = 1;
				}
				else
				{
					CONTROL_ButtonClickedState[i] = 1;
					CONTROL_ButtonClickedTime[i] = 0;
					CONTROL_ButtonClickedCount[i] = 2;
				}
			}
			else if (CONTROL_ButtonClickedCount[i] == 2)
				CONTROL_ButtonClickedState[i] = 1;
		}
		else
		{
			if (CONTROL_ButtonClickedCount[i] == 2) CONTROL_ButtonClickedCount[i] = 0;
			CONTROL_ButtonClicked[i] = 0;
		}
	}
}

	// Analog to digital with hysteresis: beyond 512 always, beyond 128 if it was already on.
static void CONTROL_DigitizeAxis(int32 axis)
{
	controlaxistype *a = &CONTROL_Axes[axis], *l = &CONTROL_LastAxes[axis];

	if (a->amount > 0)
	{
		if (a->amount > 512) a->digital = 1;
		else if (a->amount > 128 && l->digital == 1) a->digital = 1;
	}
	else
	{
		if (a->amount < -512) a->digital = -1;
		else if (a->amount < -128 && l->digital == -1) a->digital = -1;
	}
}

static void CONTROL_ApplyAxis(int32 axis, ControlInfo *info)
{
	switch (CONTROL_AxesMap[axis].analogmap)
	{
		case analog_turning:          info->dyaw += CONTROL_Axes[axis].amount; break;
		case analog_strafing:         info->dx += CONTROL_Axes[axis].amount; break;
		case analog_lookingupanddown: info->dpitch += CONTROL_Axes[axis].amount; break;
		case analog_elevation:        info->dy += CONTROL_Axes[axis].amount; break;
		case analog_rolling:          info->droll += CONTROL_Axes[axis].amount; break;
		case analog_moving:           info->dz += CONTROL_Axes[axis].amount; break;
	}
}

static void CONTROL_PollDevices(ControlInfo *info)
{
	int32 i;

	memcpy(CONTROL_LastAxes, CONTROL_Axes, sizeof(CONTROL_Axes));
	memset(CONTROL_Axes, 0, sizeof(CONTROL_Axes));
	memset(info, 0, sizeof(ControlInfo));

	if (CONTROL_MouseEnabled) CONTROL_GetMouseDelta(&CONTROL_Axes[0].amount, &CONTROL_Axes[1].amount);

	for (i = 0; i < CONTROL_NumAxes; i++)
	{
		CONTROL_DigitizeAxis(i);
		CONTROL_Axes[i].amount = (int32)(((long long)CONTROL_Axes[i].amount * CONTROL_AxesScale[i]) >> 16);
		if (CONTROL_Axes[i].amount > 0x7fff) CONTROL_Axes[i].amount = 0x7fff;
		if (CONTROL_Axes[i].amount < -0x7fff) CONTROL_Axes[i].amount = -0x7fff;
		CONTROL_ApplyAxis(i, info);
	}
	CONTROL_GetDeviceButtons();
}

static void CONTROL_AxisFunctionState(int32 *p1)
{
	int32 i, j;

	for (i = 0; i < CONTROL_NumAxes; i++)
	{
		if (CONTROL_Axes[i].digital > 0) j = CONTROL_AxesMap[i].minmap;
		else if (CONTROL_Axes[i].digital < 0) j = CONTROL_AxesMap[i].maxmap;
		else continue;
		if (j != UNMAPPED) p1[j] = 1;
	}
}

static void CONTROL_ButtonFunctionState(int32 *p1)
{
	int32 i, j;

	for (i = 0; i < CONTROL_NumButtons; i++)
	{
		j = CONTROL_DeviceButtonMapping[i].doubleclicked;
		if (j != UNMAPPED) p1[j] |= CONTROL_ButtonClickedState[i];
		j = CONTROL_DeviceButtonMapping[i].singleclicked;
		if (j != UNMAPPED) p1[j] |= CONTROL_DeviceButtonState[i];
	}
}

void CONTROL_ClearButton(int32 whichbutton)
{
	CONTROL_CheckRange(whichbutton);
	if (whichbutton > 31) CONTROL_ButtonState2 &= ~(1u << (whichbutton - 32));
	else CONTROL_ButtonState1 &= ~(1u << whichbutton);
	CONTROL_Flags[whichbutton].cleared = 1;
}

void CONTROL_GetInput(ControlInfo *info)
{
	int32 periphs[MAXFUNCTIONS];
	int32 i;

	CONTROL_PollDevices(info);

	memset(periphs, 0, sizeof(periphs));
	CONTROL_ButtonFunctionState(periphs);
	CONTROL_AxisFunctionState(periphs);

	CONTROL_ButtonHeldState1 = CONTROL_ButtonState1;
	CONTROL_ButtonHeldState2 = CONTROL_ButtonState2;
	CONTROL_ButtonState1 = 0;
	CONTROL_ButtonState2 = 0;

	for (i = 0; i < MAXFUNCTIONS; i++)
	{
		CONTROL_SetFlag(i, CONTROL_KeyboardFunctionPressed(i) | periphs[i]);
		if (CONTROL_Flags[i].cleared)
		{
			if (!CONTROL_Flags[i].active) CONTROL_Flags[i].cleared = 0;
		}
		else if (i > 31) CONTROL_ButtonState2 |= (uint32)CONTROL_Flags[i].active << (i - 32);
		else CONTROL_ButtonState1 |= (uint32)CONTROL_Flags[i].active << i;
	}
}

	// Joystick calibration callbacks (DOS game port): nothing to calibrate.
void CONTROL_CenterJoystick(void (*CenterCenter)(void), void (*UpperLeft)(void), void (*LowerRight)(void),
	void (*CenterThrottle)(void), void (*CenterRudder)(void))
{
}

void CONTROL_Startup(controltype which, int32 (*TimeFunction)(void), int32 ticspersecond)
{
	int32 i, wantmouse = 0;

	if (CONTROL_Started) return;

	GetTime = TimeFunction ? TimeFunction : CONTROL_GetTime;
	ticrate = ticspersecond;
	CONTROL_DoubleClickSpeed = (ticspersecond * 57) / 100;
	if (CONTROL_DoubleClickSpeed < 1) CONTROL_DoubleClickSpeed = 1;

	if (which == controltype_keyboardandmouse) { CONTROL_NumAxes = 2; wantmouse = 1; }

	CONTROL_MousePresent = 0;
	if (wantmouse)
	{
		CONTROL_NumButtons = 3;
		CONTROL_MousePresent = MOUSE_Init();
	}
	CONTROL_MouseEnabled = CONTROL_MousePresent;
	if (CONTROL_MousePresent) printf("CONTROL_Startup: Mouse Present\n");

	for (i = 0; i < MaxJoys; i++) CONTROL_JoysPresent[i] = 0;

	CONTROL_ButtonState1 = CONTROL_ButtonState2 = 0;
	CONTROL_ButtonHeldState1 = CONTROL_ButtonHeldState2 = 0;
	for (i = 0; i < MAXFUNCTIONS; i++) CONTROL_Flags[i].used = 0;

	CONTROL_Started = 1;
}

void CONTROL_Shutdown(void)
{
	if (!CONTROL_Started) return;
	CONTROL_JoystickEnabled = 0;
	CONTROL_Started = 0;
}
