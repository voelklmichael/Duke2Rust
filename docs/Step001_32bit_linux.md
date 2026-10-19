# Step 001 – Port the C code to 32-bit Linux

Goal: a native Linux build of Duke Nukem 3D, compiled with gcc **and** clang
using `-m32`, that runs the original game data. This gets the code ready for
c2rust (Step 002), which needs C that clang can compile for a Linux target.

Non-goals for this step: cleaning up the code, 64-bit support, networking, and
perfect audio.

## Why 32-bit

The code assumes `sizeof(long) == 4` and `sizeof(void*) == 4` throughout:

- pointers are stored in `long` variables (`waloff[]`, `frameplace`,
  `palookup` offsets, ...)
- `.MAP`/`.ART`/savegame data is `read()` straight into structs made of `long`

With `-m32`, the code behaves as it did on DOS. c2rust will then produce
`c_long == i32` for the `i686-unknown-linux-gnu` target, which Rust fully
supports. Moving to 64-bit is cheaper later, once the code is in Rust.

## Starting point (inventory)

| Part | Source | Status |
|---|---|---|
| Game (`GAME.C`, `ACTORS.C`, ...) | `duke_nukem_3d/source` | C, DOS/Watcom specific in places |
| Build engine (`ENGINE.C`, `CACHE1D.C`, `MMULTI.C`) | `BuildEngine.zip` → `SRC.ZIP` | C, **Ken's 2000 release, not the 1996 engine Duke shipped with** |
| `A.ASM` (texture-mapping inner loops) | `SRC.ZIP` | WASM assembly → rewrite in C |
| `PRAGMAS.H` (~180 `#pragma aux` inline-asm math helpers) | game + engine | Watcom inline asm → rewrite as C functions |
| MACT (`MACT386.LIB`: CONTROL/KB/MOUSE/SCRIPT/util) | **no source** | reimplement, ~55 functions (list below) |
| Audio (`AUDIO_WF.LIB`: FX_*/MUSIC_*) | `AudioLib.zip` (DOS hardware code) | stub first, ~28 functions |
| Video, keyboard ISR, timer ISR, DOS calls | engine + game | replace with SDL2 |

Functions the game imports from MACT (taken from the DOS link):
`CheckParm`, `CONTROL_*` (ButtonState1/2, CenterJoystick, ClearAssignments,
ClearButton, DefineFlag, GetInput, Get/SetMouseSensitivity, JoystickEnabled,
JoystickPort, MapAnalogAxis, MapButton, MapDigitalAxis, MapKey, MousePresent,
RudderEnabled, SetAnalogAxisScale, Shutdown, Startup), `Error`,
`FindDistance2D/3D`, `IntelLong`, `KB_*` (ClearKeysDown, FlushKeyBoardQueue,
Getch, KeyDown, KeyWaiting, Shutdown, Startup, StringToScanCode),
`MOUSE_GetButtons`, `RegisterShutdownFunction`, `Safe*` (FileExists, Free,
Malloc, OpenRead, Read, Realloc), `SCRIPT_*` (Entry, Free, GetDoubleString,
GetNumber, GetString, Load, NumberEntries, PutNumber, PutString, Save),
`Z_AvailHeap`.

Functions the game imports from the audio library: `FX_*` (ErrorString,
Get/SetReverseStereo, Init, Pan3D, PlayLoopedVOC/WAV, PlayVOC3D/WAV3D,
SetCallBack, SetReverb, SetReverbDelay, SetupSoundBlaster, SetVolume,
Shutdown, StopAllSounds, StopSound, VoiceAvailable), `MUSIC_*` (Continue,
ErrorCode, ErrorString, Init, Pause, PlaySong, RegisterTimbreBank, Shutdown,
StopSong), `Music_SetVolume`.

### Finding: Ken's 2000 engine vs Duke's 1996 engine

I compiled Ken's engine source with Open Watcom and linked it in place of
Duke's prebuilt `ENGINE.OBJ` (experiment only, not committed):

- It compiles against Duke's `BUILD.H` (`MAXTILES 6144`, no `#pragma pack`)
  once Ken's `validmode*` declarations are added, and it links with no
  unresolved symbols. The function signatures the game calls match.
- `setgamemode()` only supports video options 1 (VESA), 2 (buffered 320×200)
  and 6. Duke's chained/Tseng/S3 modes (0, 3, 4, 5) were dropped, so the
  default config fails. This doesn't matter on Linux, where we replace video
  output entirely.
- With ScreenMode 2 the game still didn't reach the menu in DOSBox. Each DOSBox
  try takes 2–3 minutes and gives no debug output, so I stopped there and am
  debugging on Linux instead. [Chocolate Duke Nukem 3D](https://github.com/fabiensanglard/chocolate_duke3D)
  and [jfduke3d](https://github.com/jonof/jfduke3d) both adapted this engine to
  Duke and can serve as references when we find differences.

## Layout

The original files under `duke_nukem_3d/` stay untouched, so the DOS build
from Step 000 keeps working as the reference. The Linux port is a separate tree:

```
port/
  Makefile            # gcc or clang, -m32 → build/linux/duke3d
  game/               # Duke sources, lowercased, LF line endings, then patched
  engine/             # Ken's engine.c, cache1d.c, mmulti.c (+ a.c replacing A.ASM)
  compat/             # pragmas.h in C, DOS shims (dos.h, conio, int386 stubs ...)
  mact/               # MACT reimplementation (control, keyboard, mouse, scriplib, util)
  audio/              # FX_/MUSIC_ API: stubs, later SDL audio
  platform/           # SDL2: video (8-bit → RGBA), palette, timer, keyboard, mouse
```

The first commit only copies the original files in (lowercased, CRLF→LF). That
way `git diff` / `git log -p` on `port/` shows every porting change on its own.

## Milestones

1. **Import**: copy the sources into `port/` unmodified (one commit).
2. **Compile**: every `.c` compiles with `gcc -m32` (no link yet):
   - replace `PRAGMAS.H` with plain C (`mulscale*`, `divscale*`, `dmulscale*`,
     `klabs`, `ksgn`, `clearbuf`, `copybuf`, `qinterpolatedown16`, ...).
     Bit-exact semantics: 64-bit intermediates, then truncate to 32-bit.
   - DOS headers/functions (`dos.h`, `bios.h`, `conio.h`, `i86.h`, `int386`,
     `outp`, `inp`, `_dos_*`, `FP_OFF`, `__interrupt`) → `compat/` shims or `#ifdef`.
   - file I/O: `O_BINARY`, `strupr`, `filelength`, ... → `compat/`.
   - `#pragma pack(1)` for structs that are read from files.
3. **Link**: provide every missing symbol: `a.c`, MACT, audio stubs, and a
   null platform (no window).
4. **Headless run**: the game starts, loads `DUKE3D.GRP`, compiles the CON
   scripts and plays the attract-mode demo without crashing. It can write
   frames as PPM files (`-dumpframes`), which makes it testable without a
   display.
5. **SDL2**: a window with the 8-bit framebuffer and palette, keyboard and
   mouse input, and a 120 Hz game timer. The game is playable.
6. **clang**: builds warning-clean enough with `clang -m32`; generate
   `compile_commands.json` for c2rust.
7. **Audio (optional within this step)**: sound effects via SDL audio,
   reusing the VOC/WAV handling and the mixer logic from the AudioLib source.
   Music (MIDI) is out of scope for now.

## Verification

- Visual: compare against screenshots of the DOS build
  (`scripts/dos_snapshot.sh` takes DOSBox screenshots automatically).
- Determinism (prepared here, used fully in Step 002+): the demos (`DEMO1.DMO`,
  `DEMO2.DMO`) replay recorded input. Logging a hash of the game state each
  tick during demo playback gives an objective comparison between the C port
  and the later Rust version.

## Tooling notes

- 32-bit SDL2: the runtime (`libsdl2-2.0-0:i386`) is installed. For headers
  and the link symlink, install `libsdl2-dev:i386`. Until then the Makefile
  links `/usr/lib/i386-linux-gnu/libSDL2-2.0.so.0` directly.
- `gcc-multilib` provides the 32-bit libc.

## Progress log

- Plan written. DOS experiment with Ken's engine source done (see finding above).
