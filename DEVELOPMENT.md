# Development Guide for SonicMuff Plugin

This document provides guidance for developers working on the SonicMuff plugin.

## Architecture Overview

### Signal Flow

```
Audio Input
    ↓
[Coupling high-pass ~250Hz] → [Diode clip stage 1, driven by SUSTAIN]
    ↓
[Coupling high-pass ~700Hz] → [Diode clip stage 2, driven by SUSTAIN]
    ↓
[Tone stack: bass leg (~300Hz low-pass) / treble leg (~1kHz high-pass), blended by TONE]
    ↓
[VOLUME]
    ↓
Audio Output
```

## Core Components

### 1. **BigMuffDSP.h**
Pure math, no JUCE dependency (only `<cmath>`): the processor owns state, this header
is free functions the processor drives, testable entirely on its own.

- `softClip()` — symmetric diode-pair clipper (`tanh`); matched silicon diodes to
  ground on both gain stages give the Muff's characteristic symmetric squashing
- `onePoleCoefficient()` — `exp(-2*pi*fc/fs)`, the shared one-pole filter coefficient
- `OnePoleHighPass` / `OnePoleLowPass` — tiny per-instance filter state structs
- `ChannelState` — one channel's full filter memory (two coupling high-passes plus the
  tone stack's bass/treble legs)
- `process()` — the full per-sample signal path: coupling high-pass → driven clip →
  coupling high-pass → driven clip → tone stack blend

The tone stack's bass and treble legs sit at **different**, fixed cutoffs (300 Hz /
1000 Hz) rather than a matched complementary pair, so frequencies between them are
attenuated by both legs regardless of the TONE knob — the Muff's signature mid-scoop,
which tilts bass-vs-treble as the knob turns but never fully disappears.

### 2. **PluginProcessor.h / PluginProcessor.cpp**
The heart of the plugin. Inherits from `juce::AudioProcessor`.

**Key Responsibilities:**
- Parameter management via `AudioProcessorValueTreeState (apvts)`
- DSP process loop in `processBlock()`
- State serialization (preset save/load)
- Conditional UI creation based on `ELK_HEADLESS` flag

**Real-Time Safety (Elk OS):**
- No dynamic allocations in `processBlock()` — `channelStates` is sized once in
  `prepareToPlay()` off the reported input channel count
- `LinearSmoothedValue<float>` prevents clicks on SUSTAIN/TONE/VOLUME changes
- Filter coefficients are computed once per sample-rate change (`prepareToPlay()`),
  not per sample or per block

**Process Loop (`processBlock`):**
1. Return early if `BYPASS` is on (default)
2. Advance the smoothed SUSTAIN/TONE/VOLUME targets once per sample
3. Run each channel's sample through `BigMuff::process()`
4. Apply the smoothed VOLUME gain

### 3. **PluginEditor.h / PluginEditor.cpp** (Desktop Only)
Conditional compilation: **Only compiled when `ELK_HEADLESS=0`**

Custom dark editor (`SonicMuffAudioProcessorEditor`), not the generic JUCE editor:
- `MuffRotaryLook` — custom `LookAndFeel_V4` for the rotary knobs
- `setupSlider()` — shared styling helper for each control
- SUSTAIN/TONE/VOLUME rotary knobs and a BYPASS toggle, all bound to `apvts` via
  `SliderAttachment` / `ButtonAttachment`
- Layout lives in `resized()`; colours/fonts are set per-control in `setupSlider()`

## Building & Testing

### Debug Build
```bash
cmake --preset default
cmake --build --preset default --config Debug
```
- Creates VST3, LV2, Standalone
- Includes GUI
- Larger binary, full debug symbols

### Release Build
```bash
cmake --preset release
cmake --build --preset release
```
- Optimized code
- Smaller binary
- Production-ready

### Elk Audio OS Build
```bash
cmake --preset elk-headless
cmake --build --preset elk-headless
```
- No GUI code compiled
- Minimal footprint
- Real-time safe

### CI

`.github/workflows/build-linux.yml`, `build-macos.yml` and `build-windows.yml` each
configure and build a Release Standalone on their platform (Ninja on Linux/macOS, the
default MSVC generator on Windows) and upload the result as a build artifact.
`tests.yml` runs the GoogleTest suite on Linux and macOS on every push to `main` and
every pull request.

## Extending the Plugin

### Adding a New Parameter

1. **Add to `createParameterLayout()`** in `PluginProcessor.cpp`:
```cpp
layout.add(std::make_unique<juce::AudioParameterFloat>(
    "PARAM_ID",      // Unique identifier
    "Display Name",  // Human-readable label
    0.0f, 10.0f,     // Min, Max
    5.0f             // Default
));
```

2. **Create a smoothed value** in `PluginProcessor.h` if it needs click-free ramping:
```cpp
juce::LinearSmoothedValue<float> smoothedNewParam;
```

3. **Initialize in `prepareToPlay()`**:
```cpp
smoothedNewParam.reset(sampleRate, 0.02); // 20ms smoothing
```

4. **Use in `processBlock()`**:
```cpp
smoothedNewParam.setTargetValue(*apvts.getRawParameterValue("PARAM_ID"));
float value = smoothedNewParam.getNextValue();
```

### Running Unit Tests

Tests live in `test/` (GoogleTest, fetched via CPM) and cover the clipping/tone-stack
math (`BigMuffDSP`) and the processor (`PluginProcessor`: parameter defaults, bypass
pass-through, `processBlock()` output sanity, state save/load round-trip).

```bash
cmake --preset default-with-tests
cmake --build --preset default-with-tests
ctest --preset default-with-tests
```

The test target compiles both `PluginProcessor.cpp` and `PluginEditor.cpp` with
`ELK_HEADLESS=0`, since `createEditor()` constructs the real
`SonicMuffAudioProcessorEditor` — even though no test exercises the editor
directly, it has to be compiled and linked for the test binary to build.

### Adding a New Test

1. Add a `.cpp` file under `test/` with `TEST(...)` / `TEST_P(...)` cases
2. List it in `test/CMakeLists.txt`'s `add_executable(SonicMuffTests ...)` sources

### Adding a New UI Control

To add a new control to the existing custom editor (`PluginEditor.h/.cpp`):

1. Declare the component (and label, if any) in `PluginEditor.h`
2. Style it via `setupSlider()`, or add a new helper for other component types (see
   `bypassButton` setup for a `ToggleButton` example)
3. Bind it to `apvts` with the matching attachment type
   (`SliderAttachment` / `ButtonAttachment`)
4. Position it in `resized()`

## Performance Considerations

### Real-Time Safety Checklist
- [ ] No `new` / `delete` in `processBlock()`
- [ ] No `std::vector::resize()` in audio loop
- [ ] No mutex locks in audio thread
- [ ] No file I/O in audio thread
- [ ] Use `juce::LinearSmoothedValue` for smooth parameter changes
- [ ] Use `juce::ScopedNoDenormals` to prevent CPU overhead from denormalized floats

### Optimization Tips
1. **Filter coefficients**: `onePoleCoefficient()` calls involve `std::exp()`; they're
   computed once in `prepareToPlay()` (on sample-rate change), never per sample.
2. **Latency**: unlike a pitch-shifter, this signal chain is zero-latency — every stage
   is a simple IIR/waveshaper with no lookahead or blockwise buffering.

## Elk Audio OS Integration

### Mapping Parameters to Hardware
In Sushi's config.json:
```json
{
  "control_interface": "osc",
  "osc_server_port": 7890,
  "plugins": [
    {
      "uid": "sonicmuff",
      "path": "path/to/plugin.so",
      "parameters": [
        { "id": "SUSTAIN", "gpio": 3 },
        { "id": "TONE", "gpio": 4 }
      ]
    }
  ]
}
```

### OSC Control Example
```bash
oscsend localhost 7890 /parameter/sonicmuff/SUSTAIN f 0.8
```

## Common Issues & Solutions

### Plugin not loading in Ardour
- Check that VST3 plugin is in `~/.vst3/`
- Verify it was built for your platform (64-bit)
- Check Ardour's plugin browser for errors

### Elk Audio OS crashes
- Enable real-time kernel monitoring
- Check for allocations in `processBlock()`
- Use `juce::Logger` to debug (output goes to Elk logs)

### Parameter automation not working
- Verify parameter IDs match in `createParameterLayout()` and `processBlock()`
- Check that `apvts` is properly initialized in constructor
- Ensure `getStateInformation()` and `setStateInformation()` are implemented

## Code Style

This project follows:
- **C++23** modern conventions
- **JUCE style guide** (snake_case for variables, camelCase for methods)
- **clang-format** configuration in `.clang-format`

Run formatter:
```bash
clang-format -i Source/*.cpp Source/*.h
```

---

**For questions or contributions, refer to the main README.md**
