#include "PluginProcessor.h"
#include <cmath>
#include <gtest/gtest.h>

namespace
{
    juce::AudioBuffer<float> makeTestBuffer (int numChannels, int numSamples, float value)
    {
        juce::AudioBuffer<float> buffer (numChannels, numSamples);
        for (int ch = 0; ch < numChannels; ++ch)
            for (int i = 0; i < numSamples; ++i)
                buffer.setSample (ch, i, value);
        return buffer;
    }
}

TEST (PluginProcessorTest, ParameterLayoutHasExpectedDefaults)
{
    SonicMuffAudioProcessor processor;

    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("SUSTAIN"), 0.5f);
    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("TONE"), 0.5f);
    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("VOLUME"), 0.5f);
    EXPECT_GT (*processor.apvts.getRawParameterValue ("BYPASS"), 0.5f);
}

TEST (PluginProcessorTest, BypassedProcessBlockLeavesBufferUnchanged)
{
    SonicMuffAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);

    auto buffer = makeTestBuffer (2, 512, 0.5f);
    juce::MidiBuffer midi;
    processor.processBlock (buffer, midi);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            EXPECT_FLOAT_EQ (buffer.getSample (ch, i), 0.5f);
}

TEST (PluginProcessorTest, ProcessBlockProducesFiniteOutputWhenActive)
{
    SonicMuffAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);
    processor.apvts.getParameter ("BYPASS")->setValueNotifyingHost (0.0f);
    processor.apvts.getParameter ("SUSTAIN")->setValueNotifyingHost (0.9f);
    processor.apvts.getParameter ("TONE")->setValueNotifyingHost (0.3f);

    auto buffer = makeTestBuffer (2, 512, 0.5f);
    juce::MidiBuffer midi;

    for (int block = 0; block < 4; ++block)
        processor.processBlock (buffer, midi);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto const* data = buffer.getReadPointer (ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            ASSERT_FALSE (std::isnan (data[i]));
            ASSERT_FALSE (std::isinf (data[i]));
        }
    }
}

TEST (PluginProcessorTest, ZeroVolumeSilencesActiveOutput)
{
    SonicMuffAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);
    processor.apvts.getParameter ("BYPASS")->setValueNotifyingHost (0.0f);
    processor.apvts.getParameter ("SUSTAIN")->setValueNotifyingHost (1.0f);
    processor.apvts.getParameter ("VOLUME")->setValueNotifyingHost (0.0f);

    auto buffer = makeTestBuffer (2, 512, 0.5f);
    juce::MidiBuffer midi;

    // Two blocks: the first still ramps the smoothed VOLUME down from its default;
    // by the second block it's settled at 0.
    processor.processBlock (buffer, midi);
    processor.processBlock (buffer, midi);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            EXPECT_NEAR (buffer.getSample (ch, i), 0.0f, 1.0e-4f);
}

TEST (PluginProcessorTest, StateRoundTripsThroughGetAndSetStateInformation)
{
    SonicMuffAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);

    auto* sustainParam = processor.apvts.getParameter ("SUSTAIN");
    sustainParam->setValueNotifyingHost (0.9f);
    float const savedSustain = *processor.apvts.getRawParameterValue ("SUSTAIN");

    juce::MemoryBlock state;
    processor.getStateInformation (state);

    sustainParam->setValueNotifyingHost (0.1f);
    ASSERT_NE (*processor.apvts.getRawParameterValue ("SUSTAIN"), savedSustain);

    processor.setStateInformation (state.getData(), static_cast<int> (state.getSize()));

    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("SUSTAIN"), savedSustain);
}
