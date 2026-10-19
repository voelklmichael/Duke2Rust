#!/usr/bin/env bash
# Build the original DOS DUKE3D.EXE with Open Watcom v2 (Linux host, DOS/4GW target).
# Sources are left untouched; a lowercase-symlinked tree is created under build/dos.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/duke_nukem_3d/source"
OUT="$ROOT/build/dos"

export WATCOM="${WATCOM:-$ROOT/.tools/watcom}"
export PATH="$WATCOM/binl64:$WATCOM/binl:$PATH"
export INCLUDE="$WATCOM/h"
export EDPATH="$WATCOM/eddat"

rm -rf "$OUT"
mkdir -p "$OUT/mact"
for f in "$SRC"/*; do
  [ -f "$f" ] && ln -s "$f" "$OUT/$(basename "$f" | tr 'A-Z' 'a-z')"
done
ln -s "$SRC/mact/MACT386.LIB" "$OUT/mact/mact386.lib"

# The DOS sources include some system headers with odd casing (e.g. <fcntl.H>).
mkdir -p "$OUT/shim"
ln -s "$WATCOM/h/fcntl.h" "$OUT/shim/fcntl.H"
# OW2's <i86.h> declares sound(unsigned), which clashes with Duke's sound(short).
cat > "$OUT/shim/i86.h" <<SHIM
#define sound __ow_i86_sound
#include "$WATCOM/h/i86.h"
#undef sound
SHIM

cd "$OUT"
CFLAGS="-5r -s -otexan -imact -ishim -zq"
for c in game actors gamedef global menues player premap sector rts config animlib; do
  wcc386 $c.c $CFLAGS
done
wcc386 sounds.c -zu $CFLAGS

cat > link.lnk <<'LNK'
system dos4g
option map
option stack=90k
option nocaseexact
option quiet
name duke3d
file game,actors,gamedef,global,menues,player,premap,sector,sounds,rts,config
file engine.obj,cache1d.obj,mmulti.obj,a.obj,animlib
library audio_wf.lib,mact/mact386.lib,butwcd4.lib
LNK
wlink @link.lnk
ls -la "$OUT/duke3d.exe"
