<div align="center">

# SonicTuning — Alternate-Tuning Pitch Shifter

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
![C++](https://img.shields.io/badge/C++-23-00599C.svg?logo=cplusplus)
![JUCE](https://img.shields.io/badge/JUCE-8.0.12-orange.svg)

SonicTuning is a real-time pitch-shifting effect that lets a guitar string played in
standard tuning sound as if it were tuned to an alternate tuning — no re-stringing,
no re-tuning pegs. Pick the target tuning and which string you're playing; the plugin
shifts that string's pitch by the exact interval needed.

</div>

## UI

Dark theme with amber accents:
- Title banner up top.
- TUNING and STRING selectors.
- FINE and GAIN knobs.
- BYPASS toggle — on by default every time the plugin loads.
- Headless (Elk Audio OS) builds omit the UI entirely; only the parameters remain.

## ✨ Highlights

- Real-time pitch shifting via [Signalsmith Stretch](https://github.com/Signalsmith-Audio/signalsmith-stretch)
- 6 alternate tunings + Drop D, each string mapped to the shortest semitone shift from standard tuning
- Per-string targeting: tell the plugin which string you're playing, it shifts only by that string's interval
- Fine-tune trim in cents for intonation correction
- RT-safe DSP: no allocations in the audio thread, parameter-smoothed output gain
- Desktop GUI + headless Elk Audio OS target

## Quick Build

Clone the repo, then configure and build with CMake presets — no OS-specific script needed.

### Linux / macOS

```bash
git clone <repository-url>
cd sonictuning
cmake --preset default
cmake --build --preset default
```

Release build: `cmake --preset release && cmake --build --preset release`

### Windows

```bat
git clone <repository-url>
cd sonictuning
cmake --preset default
cmake --build --preset default --config Debug
```

Uses whatever Visual Studio version CMake detects on your machine. To pin a generator explicitly, use the `vs` preset or `ninja-debug`/`ninja-release` (needs Ninja + MSVC on `PATH`, e.g. from a "Developer PowerShell for VS").

### Headless (Elk Audio OS)

```bash
cmake --preset elk-headless
cmake --build --preset elk-headless
```

Runs on a Raspberry Pi 4 under Elk Audio OS — Signalsmith Stretch is light enough
for a single real-time instance on that hardware.

## Project Layout

- Source/: DSP and UI code (PluginProcessor, PluginEditor, TuningTable)
- cmake/: build helpers and CPM integration
- libs/: external dependencies (gitignored)

## Tunings

Each preset lists strings low to high (string 6 → string 1), same convention as
how tunings are normally written:

- Standard: E A D G B E
- Drop D: D A D G B E
- G A B D E G
- F# F# F# F# E B
- C G D G B B
- F# F# E F# E B
- C G D G C D

## Parameters

- TUNING: target tuning preset (default Standard)
- STRING: which string is being played, 6 (low) – 1 (high) (default 6)
- FINE: ±50 cents trim (default 0)
- GAIN: output level, ±24 dB (default 0 dB)
- BYPASS: on / off (default on, reset to on every load — not restored from saved state)

## License

MIT — see LICENSE.md

## Contributing

Bugs and feature requests via GitHub issues. Pull requests welcome.

---

Built with JUCE 8.0.12 | Signalsmith Stretch | C++23 | Real‑time safe
