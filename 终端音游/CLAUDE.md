# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

Single-file C++ project. No build system — compile directly:

```bash
# Linux / macOS
g++ -O2 -o main main.cpp

# Windows (MinGW)
g++ -O2 -o main.exe main.cpp
```

Run the game:
```bash
./main        # Linux/macOS
main.exe      # Windows
```

## Architecture

All game logic lives in `main.cpp`. `bits.h` is a convenience header of common STL includes.

**Core structs:**
- `Note` — a single note: `stime` (start frame), `etime` (end frame), `track` (1–4). `get_speed()` returns how many rows/frame it falls.
- `Staff` — a beatmap: name, total frame count, and a vector of Notes.
- `Display` — pre-computes a 3D boolean array `frame[1000000][10][5]` at load time (one entry per frame×row×track). `get_frame_str(i)` renders frame `i` as a string.
- `Game` — manages the list of loaded Staff objects, the main menu, and `start_play()`.

**Game loop (`start_play`):**
1. Pre-builds the entire `Display::frame` array from all notes before the loop starts.
2. Each iteration reads up to 4 non-blocking keypresses, runs hit/miss judgment, then builds the full frame into a `vector<char>` buffer using `snprintf`.
3. The buffer is flushed with a single `WriteConsoleA` (Windows) or `write` (POSIX) call — no per-line I/O.
4. Timing uses `sleep_until(next_frame)` with a fixed 8ms step for a stable 125 fps target.

**Rendering:**
- Uses ANSI VT100 escape sequences throughout (enabled on Windows via `SetConsoleMode` + `ENABLE_VIRTUAL_TERMINAL_PROCESSING`).
- Alternate screen buffer (`\033[?1049h` / `\033[?1049l`) isolates game output from the shell's scroll history.
- `\033[H` (cursor home) + overwrite — no clear-screen between frames, so there is no blank flash.
- `\033[K` after each line clears stale trailing characters; `\033[J` at frame end clears any extra rows.

**Input:** `get_key_nb()` is non-blocking — `_kbhit()/_getch()` on Windows, `fcntl O_NONBLOCK + getchar()` on POSIX. Keys F/G/H/J map to tracks 1–4.

**Judgment windows (in frames, 1 frame = 8 ms):**
- GOOD: `|dt| ≤ 10` frames (±80 ms)
- BAD: `10 < |dt| ≤ 22` frames
- MISS: note passes `dt < -30` frames without a hit

## Beatmap format

Files live in `staff/` with a `.txt` extension. Format:

```
<name>
<total_frames>
<stime> <etime> <track>
...
```

- All times are in frames (1 frame = 8 ms).
- Tracks are 1–4.
- Minimum gap between consecutive notes on the same track: > 12 frames.

## Known limitations (see 问题.md)

- Judgment uses frame counters, not real wall-clock timestamps, so true input precision is ±8 ms (one frame), not sub-millisecond.
- On Windows, `sleep_until` defaults to 15.6 ms timer resolution; add `timeBeginPeriod(1)` (link `winmm.lib`) to get ~1 ms precision.
- `Display::frame` is a 1 000 000 × 10 × 5 bool array (~50 MB stack/global); very long beatmaps will approach this limit.
