#!/usr/bin/env bash
# Run the freshly built DUKE3D.EXE in DOSBox, using the Atomic Edition game data.
# Usage: scripts/run_dos.sh [extra DUKE3D.EXE args...]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WATCOM="${WATCOM:-$ROOT/.tools/watcom}"
EXE="$ROOT/build/dos/duke3d.exe"
RUN="$ROOT/build/run"
GAME_ZIP="$ROOT/.game_content/3D_duke3d.zip"
CFG_ZIP="$ROOT/.game_content/dn3d-box.zip"

[ -f "$EXE" ] || "$ROOT/scripts/build_dos.sh"

if [ ! -f "$RUN/DUKE3D.GRP" ]; then
  mkdir -p "$RUN"
  unzip -oq "$GAME_ZIP" DUKE3D.GRP DUKE.RTS DEMO1.DMO DEMO2.DMO -d "$RUN"
  # Default config (sound blaster, 320x200); created by SETUP.EXE otherwise.
  unzip -ojq "$CFG_ZIP" DUKE3D/DUKE3D.CFG -d "$RUN"
fi

# Our build, the matching CON scripts from the source release, and the DOS extender.
cp "$EXE" "$RUN/DUKE3D.EXE"
for f in GAME.CON DEFS.CON USER.CON LOOKUP.DAT; do
  cp "$ROOT/duke_nukem_3d/testdata/$f" "$RUN/$f"
done
cp "$WATCOM/binw/dos4gw.exe" "$RUN/DOS4GW.EXE"

cat > "$RUN/dosbox.conf" <<CONF
[dosbox]
memsize=32
[cpu]
core=dynamic
cycles=max
[render]
aspect=true
[autoexec]
mount c "$RUN"
c:
DUKE3D.EXE $*
exit
CONF

exec dosbox -conf "$RUN/dosbox.conf"
