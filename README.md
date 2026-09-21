<div align="center">

# SonicMuff — Big Muff Pi Fuzz Clone

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
![C++](https://img.shields.io/badge/C++-23-00599C.svg?logo=cplusplus)
![JUCE](https://img.shields.io/badge/JUCE-8.0.12-orange.svg)

SonicMuff is a real-time emulation of the classic op-amp Big Muff Pi fuzz pedal: two
cascaded diode clipping stages feeding a passive "scooped mids" tone stack, with the
pedal's three signature knobs — Sustain, Tone, Volume.

</div>

## UI

Dark theme with violet accents:
- Title banner up top.
- SUSTAIN, TONE and VOLUME knobs.
- BYPASS toggle — on by default every time the plugin loads.
- Headless (Elk Audio OS) builds omit the UI entirely; only the parameters remain.

## ✨ Highlights

- Two cascaded soft-clipping (diode-pair) gain stages, each preceded by a coupling
  high-pass filter that mimics the small capacitors between real Big Muff stages
- Fixed-frequency scooped-mids tone stack: independent bass/treble legs blended by
  the TONE knob, so the mid notch stays put while the balance tilts
- RT-safe DSP: no allocations in the audio thread, parameter-smoothed controls
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

### CI

`.github/workflows/build-linux.yml`, `build-macos.yml` and `build-windows.yml` each
build a Release Standalone on their platform and upload it as an artifact.
`tests.yml` builds and runs the GoogleTest suite on Linux and macOS on every push/PR.

## Project Layout

- Source/: DSP and UI code (PluginProcessor, PluginEditor, BigMuffDSP)
- cmake/: build helpers and CPM integration
- libs/: external dependencies (gitignored)

## Signal Chain

```
Input
  ↓
[Coupling high-pass ~250Hz] → [Diode clip, driven by SUSTAIN] — stage 1
  ↓
[Coupling high-pass ~700Hz] → [Diode clip, driven by SUSTAIN] — stage 2
  ↓
[Tone stack: bass leg (~300Hz low-pass) / treble leg (~1kHz high-pass), blended by TONE]
  ↓
[VOLUME]
  ↓
Output
```

## Parameters

- SUSTAIN: drive into both clipping stages — more fuzz/sustain as it rises (default 0.5)
- TONE: blends the tone stack's bass leg (0) and treble leg (1) (default 0.5, "scooped")
- VOLUME: output level, 0–1 (default 0.5)
- BYPASS: on / off (default on, reset to on every load — not restored from saved state)

## License

MIT — see LICENSE.md

## Contributing

Bugs and feature requests via GitHub issues. Pull requests welcome.

---

Built with JUCE 8.0.12 | C++23 | Real‑time safe
