# Step 000 – Compiling and running the original C code

Goal: before rewriting anything in Rust, get the original Duke Nukem 3D C sources
to build and run, so there is a working reference to compare against.

## What we start with

- `duke_nukem_3d/source/` – the game code (v1.5 CD source release, 2003). The
  file names are uppercase DOS 8.3 names (`GAME.C`, `DUKE3D.H`, ...).
- Prebuilt object files and libraries that the game links against, all inside
  `source/`:
  - `ENGINE.OBJ`, `CACHE1D.OBJ`, `A.OBJ`, `MMULTI.OBJ` – the Build engine
    (source is in `BuildEngine.zip`)
  - `AUDIO_WF.LIB` – Apogee Sound System (mostly prebuilt objects in `AudioLib.zip`)
  - `mact/MACT386.LIB` – input/config/script library (**no source available**)
  - `BUTWCD4.LIB`
- `duke_nukem_3d/testdata/` – the `.CON` scripts and `LOOKUP.DAT` that match
  this source release.
- `.game_content/` (not in git) – original game zips. `3D_duke3d.zip` holds
  the Atomic Edition data (`DUKE3D.GRP`, 26 MB), which is what our build needs.

The code was written for Watcom C 10/11 and targets 32-bit DOS using the
DOS/4GW extender.

## Toolchain

[Open Watcom v2](https://github.com/open-watcom/open-watcom-v2) runs natively
on Linux and can build DOS/4GW programs. It goes in `.tools/watcom` (not in git):

```sh
mkdir -p .tools/watcom
curl -L https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/ow-snapshot.tar.xz \
  | tar -xJ -C .tools/watcom
```

To use an existing installation instead, set `WATCOM`.

## Building

```sh
scripts/build_dos.sh
```

This produces `build/dos/duke3d.exe`, a DOS LE executable. It reports itself as
"Duke Nukem 3D v1.4 - Atomic Edition"; the source release is labelled 1.5, but
that string comes from the code itself.

### Problems we hit and how they are solved

| Problem | Solution |
|---|---|
| Linux file names are case-sensitive, the sources are uppercase, the makefile uses lowercase | The script builds in `build/dos/`, which holds lowercase symlinks to the original files. `source/` is never modified by the build. |
| `#include <fcntl.H>` (capital `.H`) is not found | Shim header `build/dos/shim/fcntl.H` → Watcom's `fcntl.h` |
| OW2's `<i86.h>` declares `sound(unsigned)`, which conflicts with Duke's `sound(short)` | Shim `build/dos/shim/i86.h` renames the OW declaration before including the real header |
| Linking fails with many unresolved symbols: the prebuilt `A.OBJ` uses uppercase names (`_PALOOKUPOFFSE`, ...) | The Watcom 11 linker ignored case and OW2's doesn't: link with `option nocaseexact` |
| The original `MAKEFILE.LNK` uses `option u`, which hides unresolved symbols and produces an executable that crashes | Removed. The link now finishes with no unresolved symbols. |

Source changes were needed for three type mismatches. OW2 treats these as
errors; Watcom 11 only warned:

- `DUKE3D.H:460` – `extern void TestCallBack(unsigned long)` → `long`, to
  match the definition in `SOUNDS.C` and `FUNCT.H`. The function relies on
  `num < 0`, so it must stay signed.
- `SOUNDS.C:102` – cast `TestCallBack` to `void (*)(unsigned long)` when passing
  it to `FX_SetCallBack`, which the audio library declares that way. `long`
  and `unsigned long` are passed the same way, so behaviour doesn't change.
- `RTS.C:95` – `SafeRealloc((void **)&lumpinfo, ...)`.

The compiler still prints many warnings (missing prototypes, shift amounts).
They are harmless for now and come from the original code.

## Running

Requires DOSBox (tested with 0.74-3 from the Ubuntu repositories).

```sh
scripts/run_dos.sh            # arguments are passed through to DUKE3D.EXE
```

The script builds the executable first if it is missing, then sets up `build/run/`:

- the first time only, extracts `DUKE3D.GRP`, `DUKE.RTS` and the demos from
  `.game_content/3D_duke3d.zip`
- the first time only, takes `DUKE3D.CFG` from `.game_content/dn3d-box.zip`,
  since the Atomic zip has none. This config is for v1.3D, uses Sound Blaster
  and 320×200.
- every time, copies in our `duke3d.exe`, the `.CON` files from
  `duke_nukem_3d/testdata/` (they match the source; the ones in the game zip
  are older), and Open Watcom's `binw/dos4gw.exe` as the DOS extender
- writes `build/run/dosbox.conf` (32 MB RAM, dynamic core, max cycles) and
  starts DOSBox with it. DOSBox 0.74 doesn't support `-set` on the command
  line, which is why the script writes a config file.

Result: the game starts and runs in DOSBox.

## Open points

- DOSBox reports no MIDI device, so music may be silent.
- The borrowed `DUKE3D.CFG` is for v1.3D. If settings misbehave, generate a
  fresh one with the game's `SETUP.EXE`.
- `MACT386.LIB` has no source. A Rust port has to reimplement it from its
  headers (`CONTROL.H`, `KEYBOARD.H`, `SCRIPLIB.H`, `UTIL_LIB.H`, ...) and how
  the game uses it.
