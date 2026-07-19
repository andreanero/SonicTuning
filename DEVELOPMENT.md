# Development Guide for SonicTuning Plugin

This document provides guidance for developers working on the SonicTuning plugin.

## Architecture Overview

### Signal Flow

```
Audio Input
    ↓
[Signalsmith Stretch: transpose by semitoneShiftForString(TUNING, STRING) + FINE/100]
    ↓
[Smoothed Output Gain]
    ↓
Audio Output
```

## Core Components

### 1. **TuningTable.h**
- Defines `kTuningPresets`: 7 tunings (Standard + Drop D + 5 alternates), each as
  6 pitch classes ordered low to high (string 6 → string 1)
- `shortestSemitoneShift()` — the signed semitone path (in [-6, 6]) from one pitch
  class to another, taking the shorter direction. Mirrors how a guitarist actually
  retunes a string (nudge a few semitones, never a whole octave)
- `semitoneShiftForString(tuningIndex, stringIndex)` — the shift to apply for a
  given tuning/string combination, relative to standard tuning
- `tuningChoices()` / `stringChoices()` — display strings for the APVTS choice
  parameters and the editor's combo boxes

### 2. **PluginProcessor.h / PluginProcessor.cpp**
The heart of the plugin. Inherits from `juce::AudioProcessor`.

**Key Responsibilities:**
- Parameter management via `AudioProcessorValueTreeState (apvts)`
- DSP process loop in `processBlock()`
- State serialization (preset save/load)
- Conditional UI creation based on `ELK_HEADLESS` flag

**Real-Time Safety (Elk OS):**
- No dynamic allocations in `processBlock()` — `signalsmith::stretch::SignalsmithStretch`
  is configured once in `prepareToPlay()` via `presetDefault()`, and the scratch
  `wetBuffer` is sized there too
- `LinearSmoothedValue<float>` prevents gain clicks
- Latency is reported to the host via `setLatencySamples()` in `prepareToPlay()`,
  using `stretch.inputLatency() + stretch.outputLatency()`

**Process Loop (`processBlock`):**
1. Return early if `BYPASS` is on (default)
2. Compute the target transpose in semitones: `semitoneShiftForString(TUNING, STRING) + FINE / 100`
3. `stretch.process()` the input into `wetBuffer`
4. Copy `wetBuffer` back into the main buffer, applying smoothed `GAIN`

### 3. **PluginEditor.h / PluginEditor.cpp** (Desktop Only)
Conditional compilation: **Only compiled when `ELK_HEADLESS=0`**

Custom dark editor (`SonicTuningAudioProcessorEditor`), not the generic JUCE editor:
- `TuningRotaryLook` — custom `LookAndFeel_V4` for the rotary knobs
- `setupSlider()` / `setupComboBox()` — shared styling helpers for each control
- TUNING/STRING combo boxes, FINE/GAIN rotary knobs, and a BYPASS toggle, all bound
  to `apvts` via `ComboBoxAttachment` / `SliderAttachment` / `ButtonAttachment`
- Layout lives in `resized()`; colours/fonts are set per-control in the `setup*()` helpers

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
- Real-time safe; validated as a single-instance target on Raspberry Pi 4

## Extending the Plugin

### Adding a New Tuning

1. **Add a preset to `kTuningPresets`** in `TuningTable.h`, listing pitch classes
   low to high (string 6 → string 1):
```cpp
{ "New Tuning Name", { PitchClass::E, PitchClass::A, PitchClass::D, PitchClass::G, PitchClass::B, PitchClass::E } },
```
2. `tuningChoices()` picks it up automatically — no other change needed, since the
   `TUNING` parameter's choice list and `semitoneShiftForString()` both read from
   `kTuningPresets`.

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

Tests live in `test/` (GoogleTest, fetched via CPM) and cover the tuning math
(`TuningTable`) and the processor (`PluginProcessor`: parameter defaults, bypass
pass-through, `processBlock()` output sanity, state save/load round-trip).

```bash
cmake --preset default-with-tests
cmake --build --preset default-with-tests
ctest --preset default-with-tests
```

The test target compiles both `PluginProcessor.cpp` and `PluginEditor.cpp` with
`ELK_HEADLESS=0`, since `createEditor()` constructs the real
`SonicTuningAudioProcessorEditor` — even though no test exercises the editor
directly, it has to be compiled and linked for the test binary to build.

### Adding a New Test

1. Add a `.cpp` file under `test/` with `TEST(...)` / `TEST_P(...)` cases
2. List it in `test/CMakeLists.txt`'s `add_executable(SonicTuningTests ...)` sources

### Adding a New UI Control

To add a new control to the existing custom editor (`PluginEditor.h/.cpp`):

1. Declare the component (and label, if any) in `PluginEditor.h`
2. Style it via `setupSlider()` / `setupComboBox()`, or add a new helper for other
   component types (see `bypassButton` setup for a `ToggleButton` example)
3. Bind it to `apvts` with the matching attachment type
   (`SliderAttachment` / `ComboBoxAttachment` / `ButtonAttachment`)
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
1. **Pitch shift quality vs. CPU**: `stretch.presetDefault()` favors quality; switch
   to `stretch.presetCheaper()` if CPU headroom is tight on embedded targets.
2. **Latency**: Signalsmith Stretch has inherent algorithmic latency
   (`inputLatency() + outputLatency()`), reported to the host via
   `setLatencySamples()`. There's no way to shift pitch in real time with zero
   latency — budget for it in live rigs.

## Elk Audio OS Integration

### Mapping Parameters to Hardware
In Sushi's config.json:
```json
{
  "control_interface": "osc",
  "osc_server_port": 7890,
  "plugins": [
    {
      "uid": "sonictuning",
      "path": "path/to/plugin.so",
      "parameters": [
        { "id": "TUNING", "gpio": 3 },
        { "id": "STRING", "gpio": 4 }
      ]
    }
  ]
}
```

### OSC Control Example
```bash
oscsend localhost 7890 /parameter/sonictuning/TUNING f 0.33
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
