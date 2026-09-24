#pragma once

#include <cmath>

// DSP building blocks modelling the Big Muff Pi's two cascaded op-amp/diode clipping
// stages and its passive "scooped mids" tone stack. Pure math, no JUCE dependency: the
// processor owns per-channel state and drives these functions from processBlock(), so
// this header is unit-testable entirely on its own.
namespace BigMuff
{
    constexpr double kPi = 3.14159265358979323846;

    // Fixed internal voicing constants (not exposed as parameters), matched to the
    // classic op-amp Big Muff Pi topology.
    constexpr float kStageOneHighPassHz = 250.0f;    // input cap feeding clipping stage 1
    constexpr float kStageTwoHighPassHz = 700.0f;    // coupling cap between stage 1 and 2
    constexpr float kToneBassHz = 300.0f;            // tone stack bass (low-pass) leg
    constexpr float kToneTrebleHz = 1000.0f;         // tone stack treble (high-pass) leg
    constexpr float kStageOneMaxDriveGain = 60.0f;   // SUSTAIN=1 gain into stage 1
    constexpr float kStageTwoMaxDriveGain = 12.0f;   // SUSTAIN=1 gain into stage 2

    // Symmetric diode-pair clipper: matched silicon diodes to ground on both cascaded
    // gain stages give the Muff's characteristic symmetric (not fuzz-face-style
    // asymmetric) waveform squashing.
    inline float softClip (float x)
    {
        return std::tanh (x);
    }

    // exp(-2*pi*fc/fs): the one-pole coefficient shared by the high-pass and low-pass
    // filters below. Computed once per sample-rate change, not per sample.
    inline float onePoleCoefficient (float cutoffHz, double sampleRate)
    {
        return static_cast<float> (std::exp (-2.0 * kPi * (double) cutoffHz / sampleRate));
    }

    // One-pole high-pass. Models the small coupling capacitors feeding each clipping
    // stage, and the treble leg of the tone stack.
    struct OnePoleHighPass
    {
        float prevInput = 0.0f;
        float prevOutput = 0.0f;

        float process (float x, float coeff)
        {
            float const y = coeff * (prevOutput + x - prevInput);
            prevInput = x;
            prevOutput = y;
            return y;
        }

        void reset() { prevInput = prevOutput = 0.0f; }
    };

    // One-pole low-pass: the bass leg of the tone stack.
    struct OnePoleLowPass
    {
        float prevOutput = 0.0f;

        float process (float x, float coeff)
        {
            prevOutput += (1.0f - coeff) * (x - prevOutput);
            return prevOutput;
        }

        void reset() { prevOutput = 0.0f; }
    };

    // Per-channel filter memory for the full signal chain: the two inter-stage coupling
    // high-passes, plus the tone stack's bass/treble legs.
    struct ChannelState
    {
        OnePoleHighPass stageOneHighPass, stageTwoHighPass;
        OnePoleLowPass toneBass;
        OnePoleHighPass toneTreble;

        void reset()
        {
            stageOneHighPass.reset();
            stageTwoHighPass.reset();
            toneBass.reset();
            toneTreble.reset();
        }
    };

    // Full per-sample signal path: coupling high-pass -> driven diode clip (stage 1) ->
    // coupling high-pass -> driven diode clip (stage 2) -> tone stack blend.
    //
    // The tone stack's bass and treble legs are fixed at different cutoffs (300 Hz /
    // 1000 Hz) rather than a matched complementary pair, so frequencies between them are
    // attenuated by both legs no matter where `tone` sits — the Muff's signature
    // scooped-mids notch, which only tilts bass-vs-treble as the knob turns rather than
    // disappearing at any setting.
    //
    // `sustain`/`tone` are both in [0, 1]; coefficients are precomputed once per block by
    // the caller (from prepareToPlay/parameter smoothing), not recomputed per sample here.
    inline float process (float x, ChannelState& state,
                          float stageOneHpCoeff, float stageTwoHpCoeff,
                          float toneBassCoeff, float toneTrebleCoeff,
                          float sustain, float tone)
    {
        float const stageOneDrive = 1.0f + sustain * kStageOneMaxDriveGain;
        float const stageTwoDrive = 1.0f + sustain * kStageTwoMaxDriveGain;

        float const stageOneIn = state.stageOneHighPass.process (x, stageOneHpCoeff);
        float const stageOneOut = softClip (stageOneIn * stageOneDrive);

        float const stageTwoIn = state.stageTwoHighPass.process (stageOneOut, stageTwoHpCoeff);
        float const stageTwoOut = softClip (stageTwoIn * stageTwoDrive);

        float const bassLeg = state.toneBass.process (stageTwoOut, toneBassCoeff);
        float const trebleLeg = state.toneTreble.process (stageTwoOut, toneTrebleCoeff);

        return bassLeg + tone * (trebleLeg - bassLeg);
    }
}
