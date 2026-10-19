#!/usr/bin/env bash
# Launch a DOS DUKE3D.EXE in DOSBox, take window screenshots after given delays, then quit.
# Usage: scripts/dos_snapshot.sh <duke3d.exe> <outprefix> <secs>... 
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EXE="$1"; OUT="$2"; shift 2
RUN="$ROOT/build/snaprun"
"$ROOT/scripts/run_dos.sh" --prepare-only >/dev/null
rm -rf "$RUN"; cp -r "$ROOT/build/run" "$RUN"; cp "$EXE" "$RUN/DUKE3D.EXE"
sed -i -e "s#$ROOT/build/run#$RUN#" -e "/^exit$/d" "$RUN/dosbox.conf"
# Optional: SCREENMODE=<n> overrides ScreenMode in DUKE3D.CFG (Ken's 2000 engine only supports 1, 2).
[ -n "${SCREENMODE:-}" ] && sed -i "s/^ScreenMode = .*/ScreenMode = $SCREENMODE/" "$RUN/DUKE3D.CFG"
dosbox -conf "$RUN/dosbox.conf" >/dev/null 2>&1 &
PID=$!
prev=0
for t in "$@"; do
  sleep $((t - prev)); prev=$t
  WID=$(xdotool search --pid $PID 2>/dev/null | tail -1 || true)
  if [ -n "$WID" ]; then import -window "$WID" "${OUT}_${t}s.png"; else echo "no window at ${t}s"; fi
done
kill $PID 2>/dev/null || true
