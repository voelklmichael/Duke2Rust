#!/usr/bin/env bash
# Writes port/compile_commands.json (for c2rust / clangd) from the Makefile's
# compile commands, using clang. Usage: scripts/compile_commands.sh [PLATFORM=null]
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/port"
make -B -n CC=clang "$@" objs | python3 -c '
import json, os, shlex, sys
entries = []
for line in sys.stdin:
    line = line.strip()
    if not line.startswith("clang ") or " -c " not in line: continue
    args = shlex.split(line)
    src = args[args.index("-c") + 1]
    entries.append({"directory": os.getcwd(), "file": src, "arguments": args})
json.dump(entries, open("compile_commands.json", "w"), indent=2)
print(len(entries), "entries written to", os.path.join(os.getcwd(), "compile_commands.json"))
'
