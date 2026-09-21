#include "BigMuffDSP.h"
#include <cmath>
#include <gtest/gtest.h>

TEST (BigMuffDSPTest, SoftClipIsBoundedOddAndNearLinearForSmallInput)
{
    EXPECT_NEAR (BigMuff::softClip (0.0f), 0.0f, 1.0e-6f);
    EXPECT_LT (BigMuff::softClip (5.0f), 1.0f);
    EXPECT_GT (BigMuff::softClip (5.0f), 0.999f);
    EXPECT_FLOAT_EQ (BigMuff::softClip (-3.0f), -BigMuff::softClip (3.0f));
    EXPECT_NEAR (BigMuff::softClip (0.01f), 0.01f, 1.0e-4f);
}

TEST (BigMuffDSPTest, OnePoleCoefficientDecreasesAsCutoffRises)
{
    double const sampleRate = 48000.0;
    float const lowCutoffCoeff = BigMuff::onePoleCoefficient (100.0f, sampleRate);
    float const highCutoffCoeff = BigMuff::onePoleCoefficient (5000.0f, sampleRate);

    EXPECT_GT (lowCutoffCoeff, 0.0f);
    EXPECT_LT (lowCutoffCoeff, 1.0f);
    EXPECT_GT (highCutoffCoeff, 0.0f);
    EXPECT_LT (highCutoffCoeff, lowCutoffCoeff);
}

namespace
{
    struct Coeffs
    {
        float hp1, hp2, toneBass, toneTreble;
    };

    Coeffs makeCoeffs (double sampleRate)
    {
        return {
            BigMuff::onePoleCoefficient (BigMuff::kStageOneHighPassHz, sampleRate),
            BigMuff::onePoleCoefficient (BigMuff::kStageTwoHighPassHz, sampleRate),
            BigMuff::onePoleCoefficient (BigMuff::kToneBassHz, sampleRate),
            BigMuff::onePoleCoefficient (BigMuff::kToneTrebleHz, sampleRate),
        };
    }
}

TEST (BigMuffDSPTest, ProcessProducesFiniteOutputAcrossSustainAndToneRange)
{
    double const sampleRate = 48000.0;
    auto const c = makeCoeffs (sampleRate);

    for (float sustain : { 0.0f, 0.5f, 1.0f })
    {
        for (float tone : { 0.0f, 0.5f, 1.0f })
        {
            BigMuff::ChannelState state;
            for (int i = 0; i < 2000; ++i)
            {
                float const x = std::sin (2.0 * BigMuff::kPi * 220.0 * (double) i / sampleRate) * 0.8f;
                float const y = BigMuff::process (x, state, c.hp1, c.hp2, c.toneBass, c.toneTreble, sustain, tone);
                ASSERT_FALSE (std::isnan (y));
                ASSERT_FALSE (std::isinf (y));
            }
        }
    }
}

// The tone stack's bass and treble legs are computed unconditionally every sample and
// blended only at the very end (`bassLeg + tone * (trebleLeg - bassLeg)`), independently
// of the two clipping stages that precede them. So a TONE=0.5 run must land exactly
// halfway between independent TONE=0 (bass-only) and TONE=1 (treble-only) runs fed the
// same input sequence.
TEST (BigMuffDSPTest, ToneKnobLinearlyBlendsBassAndTrebleLegs)
{
    double const sampleRate = 48000.0;
    auto const c = makeCoeffs (sampleRate);

    BigMuff::ChannelState bassState, trebleState, mixState;

    for (int i = 0; i < 500; ++i)
    {
        float const x = std::sin (2.0 * BigMuff::kPi * 220.0 * (double) i / sampleRate) * 0.5f;

        float const bassOnly   = BigMuff::process (x, bassState,   c.hp1, c.hp2, c.toneBass, c.toneTreble, 0.3f, 0.0f);
        float const trebleOnly = BigMuff::process (x, trebleState, c.hp1, c.hp2, c.toneBass, c.toneTreble, 0.3f, 1.0f);
        float const mixed      = BigMuff::process (x, mixState,    c.hp1, c.hp2, c.toneBass, c.toneTreble, 0.3f, 0.5f);

        EXPECT_NEAR (mixed, 0.5f * (bassOnly + trebleOnly), 1.0e-5f);
    }
}

// SUSTAIN drives the gain feeding both diode clipping stages: at SUSTAIN=0 a quiet
// input passes through close to linearly, while at SUSTAIN=1 it's driven hard enough
// to push both tanh stages toward saturation, so the settled output level should be
// clearly higher despite the identical (quiet) input.
TEST (BigMuffDSPTest, HigherSustainIncreasesSettledOutputLevelForQuietInput)
{
    double const sampleRate = 48000.0;
    auto const c = makeCoeffs (sampleRate);

    auto settledPeak = [&] (float sustain)
    {
        BigMuff::ChannelState state;
        float peak = 0.0f;
        for (int i = 0; i < 4000; ++i)
        {
            float const x = std::sin (2.0 * BigMuff::kPi * 220.0 * (double) i / sampleRate) * 0.05f;
            float const y = BigMuff::process (x, state, c.hp1, c.hp2, c.toneBass, c.toneTreble, sustain, 0.5f);
            if (i > 3000)
                peak = std::max (peak, std::abs (y));
        }
        return peak;
    };

    EXPECT_GT (settledPeak (1.0f), settledPeak (0.0f));
}
