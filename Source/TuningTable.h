#pragma once

#include <array>
#include <juce_core/juce_core.h>

// Pitch classes, semitones above C.
namespace PitchClass
{
    constexpr int C = 0, Cs = 1, D = 2, Ds = 3, E = 4, F = 5,
                   Fs = 6, G = 7, Gs = 8, A = 9, As = 10, B = 11;
}

// One entry per string, ordered low to high (string 6 .. string 1),
// matching how tunings are conventionally written, e.g. "E-A-D-G-B-E".
struct TuningPreset
{
    const char* name;
    std::array<int, 6> pitchClasses;
};

// Presets beyond the first 7 are sourced from the Sonic Youth tuning list
// (http://www.sonicyouth.com/mustang/tab/tuninglist.html), skipping entries
// that need muted or doubled (12-string) strings our single-note-per-string
// model can't represent, and tunings already covered by the entries above.
static const std::array<TuningPreset, 86> kTuningPresets { {
    { "Standard (E A D G B E)",       { PitchClass::E,  PitchClass::A,  PitchClass::D,  PitchClass::G,  PitchClass::B,  PitchClass::E  } },
    { "Drop D (D A D G B E)",         { PitchClass::D,  PitchClass::A,  PitchClass::D,  PitchClass::G,  PitchClass::B,  PitchClass::E  } },
    { "G A B D E G",                  { PitchClass::G,  PitchClass::A,  PitchClass::B,  PitchClass::D,  PitchClass::E,  PitchClass::G  } },
    { "F# F# F# F# E B",              { PitchClass::Fs, PitchClass::Fs, PitchClass::Fs, PitchClass::Fs, PitchClass::E,  PitchClass::B  } },
    { "C G D G B B",                  { PitchClass::C,  PitchClass::G,  PitchClass::D,  PitchClass::G,  PitchClass::B,  PitchClass::B  } },
    { "F# F# E F# E B",               { PitchClass::Fs, PitchClass::Fs, PitchClass::E,  PitchClass::Fs, PitchClass::E,  PitchClass::B  } },
    { "C G D G C D",                  { PitchClass::C,  PitchClass::G,  PitchClass::D,  PitchClass::G,  PitchClass::C,  PitchClass::D  } },
    { "A A A A B D",                  { PitchClass::A,  PitchClass::A,  PitchClass::A,  PitchClass::A,  PitchClass::B,  PitchClass::D  } },
    { "A A E E F# A",                 { PitchClass::A,  PitchClass::A,  PitchClass::E,  PitchClass::E,  PitchClass::Fs, PitchClass::A  } },
    { "A A E E A A",                  { PitchClass::A,  PitchClass::A,  PitchClass::E,  PitchClass::E,  PitchClass::A,  PitchClass::A  } },
    { "A C C G G# C",                 { PitchClass::A,  PitchClass::C,  PitchClass::C,  PitchClass::G,  PitchClass::Gs, PitchClass::C  } },
    { "A F# E F# E B",                { PitchClass::A,  PitchClass::Fs, PitchClass::E,  PitchClass::Fs, PitchClass::E,  PitchClass::B  } },
    { "B E G D B B",                  { PitchClass::B,  PitchClass::E,  PitchClass::G,  PitchClass::D,  PitchClass::B,  PitchClass::B  } },
    { "B F# B B B F#",                { PitchClass::B,  PitchClass::Fs, PitchClass::B,  PitchClass::B,  PitchClass::B,  PitchClass::Fs } },
    { "B B E E A A",                  { PitchClass::B,  PitchClass::B,  PitchClass::E,  PitchClass::E,  PitchClass::A,  PitchClass::A  } },
    { "C C G D G D",                  { PitchClass::C,  PitchClass::C,  PitchClass::G,  PitchClass::D,  PitchClass::G,  PitchClass::D  } },
    { "C D G A D G",                  { PitchClass::C,  PitchClass::D,  PitchClass::G,  PitchClass::A,  PitchClass::D,  PitchClass::G  } },
    { "C G G D D D",                  { PitchClass::C,  PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::D,  PitchClass::D  } },
    { "C G C C C G",                  { PitchClass::C,  PitchClass::G,  PitchClass::C,  PitchClass::C,  PitchClass::C,  PitchClass::G  } },
    { "C G D C G D",                  { PitchClass::C,  PitchClass::G,  PitchClass::D,  PitchClass::C,  PitchClass::G,  PitchClass::D  } },
    { "C G D A C D",                  { PitchClass::C,  PitchClass::G,  PitchClass::D,  PitchClass::A,  PitchClass::C,  PitchClass::D  } },
    { "C C D# G G D",                 { PitchClass::C,  PitchClass::C,  PitchClass::Ds, PitchClass::G,  PitchClass::G,  PitchClass::D  } },
    { "C C E B G D",                  { PitchClass::C,  PitchClass::C,  PitchClass::E,  PitchClass::B,  PitchClass::G,  PitchClass::D  } },
    { "D D A E A D",                  { PitchClass::D,  PitchClass::D,  PitchClass::A,  PitchClass::E,  PitchClass::A,  PitchClass::D  } },
    { "D D A F# A D",                 { PitchClass::D,  PitchClass::D,  PitchClass::A,  PitchClass::Fs, PitchClass::A,  PitchClass::D  } },
    { "D D D D E A",                  { PitchClass::D,  PitchClass::D,  PitchClass::D,  PitchClass::D,  PitchClass::E,  PitchClass::A  } },
    { "D D D D A A",                  { PitchClass::D,  PitchClass::D,  PitchClass::D,  PitchClass::D,  PitchClass::A,  PitchClass::A  } },
    { "D D# A# D# G G",               { PitchClass::D,  PitchClass::Ds, PitchClass::As, PitchClass::Ds, PitchClass::G,  PitchClass::G  } },
    { "D D# A# D# G C",               { PitchClass::D,  PitchClass::Ds, PitchClass::As, PitchClass::Ds, PitchClass::G,  PitchClass::C  } },
    { "D E G D B B",                  { PitchClass::D,  PitchClass::E,  PitchClass::G,  PitchClass::D,  PitchClass::B,  PitchClass::B  } },
    { "D F C D F F",                  { PitchClass::D,  PitchClass::F,  PitchClass::C,  PitchClass::D,  PitchClass::F,  PitchClass::F  } },
    { "D G C D G G",                  { PitchClass::D,  PitchClass::G,  PitchClass::C,  PitchClass::D,  PitchClass::G,  PitchClass::G  } },
    { "D G D D B B",                  { PitchClass::D,  PitchClass::G,  PitchClass::D,  PitchClass::D,  PitchClass::B,  PitchClass::B  } },
    { "D A A D D A",                  { PitchClass::D,  PitchClass::A,  PitchClass::A,  PitchClass::D,  PitchClass::D,  PitchClass::A  } },
    { "D A D F# A D",                 { PitchClass::D,  PitchClass::A,  PitchClass::D,  PitchClass::Fs, PitchClass::A,  PitchClass::D  } },
    { "D A D G G B",                  { PitchClass::D,  PitchClass::A,  PitchClass::D,  PitchClass::G,  PitchClass::G,  PitchClass::B  } },
    { "D A D G B D",                  { PitchClass::D,  PitchClass::A,  PitchClass::D,  PitchClass::G,  PitchClass::B,  PitchClass::D  } },
    { "D A D A B B",                  { PitchClass::D,  PitchClass::A,  PitchClass::D,  PitchClass::A,  PitchClass::B,  PitchClass::B  } },
    { "D A D A B D",                  { PitchClass::D,  PitchClass::A,  PitchClass::D,  PitchClass::A,  PitchClass::B,  PitchClass::D  } },
    { "D A# D F C A#",                { PitchClass::D,  PitchClass::As, PitchClass::D,  PitchClass::F,  PitchClass::C,  PitchClass::As } },
    { "D# D# A A E E",                { PitchClass::Ds, PitchClass::Ds, PitchClass::A,  PitchClass::A,  PitchClass::E,  PitchClass::E  } },
    { "D# D# A# D# G G",              { PitchClass::Ds, PitchClass::Ds, PitchClass::As, PitchClass::Ds, PitchClass::G,  PitchClass::G  } },
    { "D# D# C# C# G G",              { PitchClass::Ds, PitchClass::Ds, PitchClass::Cs, PitchClass::Cs, PitchClass::G,  PitchClass::G  } },
    { "D# A# D F A# C",               { PitchClass::Ds, PitchClass::As, PitchClass::D,  PitchClass::F,  PitchClass::As, PitchClass::C  } },
    { "D# A# D# B C# F#",             { PitchClass::Ds, PitchClass::As, PitchClass::Ds, PitchClass::B,  PitchClass::Cs, PitchClass::Fs } },
    { "E E B B E F#",                 { PitchClass::E,  PitchClass::E,  PitchClass::B,  PitchClass::B,  PitchClass::E,  PitchClass::Fs } },
    { "E E B B F# F#",                { PitchClass::E,  PitchClass::E,  PitchClass::B,  PitchClass::B,  PitchClass::Fs, PitchClass::Fs } },
    { "E E B E B B",                  { PitchClass::E,  PitchClass::E,  PitchClass::B,  PitchClass::E,  PitchClass::B,  PitchClass::B  } },
    { "E E E E F# F#",                { PitchClass::E,  PitchClass::E,  PitchClass::E,  PitchClass::E,  PitchClass::Fs, PitchClass::Fs } },
    { "E E E E G# G#",                { PitchClass::E,  PitchClass::E,  PitchClass::E,  PitchClass::E,  PitchClass::Gs, PitchClass::Gs } },
    { "E E B E G# B",                 { PitchClass::E,  PitchClass::E,  PitchClass::B,  PitchClass::E,  PitchClass::Gs, PitchClass::B  } },
    { "E E B E B E",                  { PitchClass::E,  PitchClass::E,  PitchClass::B,  PitchClass::E,  PitchClass::B,  PitchClass::E  } },
    { "E E B F# B E",                 { PitchClass::E,  PitchClass::E,  PitchClass::B,  PitchClass::Fs, PitchClass::B,  PitchClass::E  } },
    { "E F# C# F# A# C#",             { PitchClass::E,  PitchClass::Fs, PitchClass::Cs, PitchClass::Fs, PitchClass::As, PitchClass::Cs } },
    { "E G D D G G",                  { PitchClass::E,  PitchClass::G,  PitchClass::D,  PitchClass::D,  PitchClass::G,  PitchClass::G  } },
    { "E G D G E D",                  { PitchClass::E,  PitchClass::G,  PitchClass::D,  PitchClass::G,  PitchClass::E,  PitchClass::D  } },
    { "E G# E G# E G#",               { PitchClass::E,  PitchClass::Gs, PitchClass::E,  PitchClass::Gs, PitchClass::E,  PitchClass::Gs } },
    { "E A D E C D",                  { PitchClass::E,  PitchClass::A,  PitchClass::D,  PitchClass::E,  PitchClass::C,  PitchClass::D  } },
    { "E B D G B E",                  { PitchClass::E,  PitchClass::B,  PitchClass::D,  PitchClass::G,  PitchClass::B,  PitchClass::E  } },
    { "E B E E A A",                  { PitchClass::E,  PitchClass::B,  PitchClass::E,  PitchClass::E,  PitchClass::A,  PitchClass::A  } },
    { "E B E E A B",                  { PitchClass::E,  PitchClass::B,  PitchClass::E,  PitchClass::E,  PitchClass::A,  PitchClass::B  } },
    { "E B E G G B",                  { PitchClass::E,  PitchClass::B,  PitchClass::E,  PitchClass::G,  PitchClass::G,  PitchClass::B  } },
    { "F G C F A F#",                 { PitchClass::F,  PitchClass::G,  PitchClass::C,  PitchClass::F,  PitchClass::A,  PitchClass::Fs } },
    { "F F C C A A",                  { PitchClass::F,  PitchClass::F,  PitchClass::C,  PitchClass::C,  PitchClass::A,  PitchClass::A  } },
    { "F F C F F G#",                 { PitchClass::F,  PitchClass::F,  PitchClass::C,  PitchClass::F,  PitchClass::F,  PitchClass::Gs } },
    { "F F C G C C",                  { PitchClass::F,  PitchClass::F,  PitchClass::C,  PitchClass::G,  PitchClass::C,  PitchClass::C  } },
    { "F F D# F A# C",                { PitchClass::F,  PitchClass::F,  PitchClass::Ds, PitchClass::F,  PitchClass::As, PitchClass::C  } },
    { "F# F# G G A A",                { PitchClass::Fs, PitchClass::Fs, PitchClass::G,  PitchClass::G,  PitchClass::A,  PitchClass::A  } },
    { "F# F# A A E E",                { PitchClass::Fs, PitchClass::Fs, PitchClass::A,  PitchClass::A,  PitchClass::E,  PitchClass::E  } },
    { "F# F# C# F# A# C#",            { PitchClass::Fs, PitchClass::Fs, PitchClass::Cs, PitchClass::Fs, PitchClass::As, PitchClass::Cs } },
    { "F# F# C# C# C# C#",            { PitchClass::Fs, PitchClass::Fs, PitchClass::Cs, PitchClass::Cs, PitchClass::Cs, PitchClass::Cs } },
    { "F# A A F# F# F#",              { PitchClass::Fs, PitchClass::A,  PitchClass::A,  PitchClass::Fs, PitchClass::Fs, PitchClass::Fs } },
    { "G G B D G A",                  { PitchClass::G,  PitchClass::G,  PitchClass::B,  PitchClass::D,  PitchClass::G,  PitchClass::A  } },
    { "G G C C A# A#",                { PitchClass::G,  PitchClass::G,  PitchClass::C,  PitchClass::C,  PitchClass::As, PitchClass::As } },
    { "G G C G C C",                  { PitchClass::G,  PitchClass::G,  PitchClass::C,  PitchClass::G,  PitchClass::C,  PitchClass::C  } },
    { "G G C G C D",                  { PitchClass::G,  PitchClass::G,  PitchClass::C,  PitchClass::G,  PitchClass::C,  PitchClass::D  } },
    { "G G C# D G G",                 { PitchClass::G,  PitchClass::G,  PitchClass::Cs, PitchClass::D,  PitchClass::G,  PitchClass::G  } },
    { "G G D D D# D#",                { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::D,  PitchClass::Ds, PitchClass::Ds } },
    { "G G D D F F",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::D,  PitchClass::F,  PitchClass::F  } },
    { "G G D D G G",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::D,  PitchClass::G,  PitchClass::G  } },
    { "G G D E G B",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::E,  PitchClass::G,  PitchClass::B  } },
    { "G G D F F G",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::F,  PitchClass::F,  PitchClass::G  } },
    { "G G D G G A",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::G,  PitchClass::G,  PitchClass::A  } },
    { "G G D G B D",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::G,  PitchClass::B,  PitchClass::D  } },
    { "G G D A G A",                  { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::A,  PitchClass::G,  PitchClass::A  } },
    { "G G D A G A#",                 { PitchClass::G,  PitchClass::G,  PitchClass::D,  PitchClass::A,  PitchClass::G,  PitchClass::As } },
} };

// Shortest signed semitone path from `standardPitchClass` to `targetPitchClass`, in [-6, 6].
// This mirrors how a guitarist actually retunes a string: nudging it up or down a few
// semitones, never re-tuning a whole octave away.
inline int shortestSemitoneShift (int standardPitchClass, int targetPitchClass)
{
    int diff = (targetPitchClass - standardPitchClass) % 12;
    if (diff > 6)
        diff -= 12;
    else if (diff < -6)
        diff += 12;
    return diff;
}

inline int semitoneShiftForString (int tuningIndex, int stringIndex)
{
    auto const& standard = kTuningPresets[0].pitchClasses;
    auto const& target = kTuningPresets[static_cast<size_t> (tuningIndex)].pitchClasses;
    return shortestSemitoneShift (standard[static_cast<size_t> (stringIndex)], target[static_cast<size_t> (stringIndex)]);
}

inline juce::StringArray tuningChoices()
{
    juce::StringArray choices;
    for (auto const& preset : kTuningPresets)
        choices.add (preset.name);
    return choices;
}

// Guitar string numbering (6 = low, 1 = high), in the same low-to-high order as kTuningPresets.
inline juce::StringArray stringChoices()
{
    return { "String 6 (Low)", "String 5", "String 4", "String 3", "String 2", "String 1 (High)" };
}