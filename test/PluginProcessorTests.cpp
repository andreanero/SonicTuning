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
    SonicTuningAudioProcessor processor;

    EXPECT_EQ (static_cast<int> (*processor.apvts.getRawParameterValue ("TUNING")), 0); // Standard
    EXPECT_EQ (static_cast<int> (*processor.apvts.getRawParameterValue ("STRING")), 0); // String 6
    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("FINE"), 0.0f);
    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("GAIN"), 0.0f);
    EXPECT_GT (*processor.apvts.getRawParameterValue ("BYPASS"), 0.5f);
}

TEST (PluginProcessorTest, BypassedProcessBlockLeavesBufferUnchanged)
{
    SonicTuningAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);

    auto buffer = makeTestBuffer (2, 512, 0.5f);
    juce::MidiBuffer midi;
    processor.processBlock (buffer, midi);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            EXPECT_FLOAT_EQ (buffer.getSample (ch, i), 0.5f);
}

TEST (PluginProcessorTest, ProcessBlockProducesFiniteBoundedOutputWhenActive)
{
    SonicTuningAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);
    processor.apvts.getParameter ("BYPASS")->setValueNotifyingHost (0.0f);
    processor.apvts.getParameter ("TUNING")->setValueNotifyingHost (2.0f / 6.0f); // "G A B D E G"
    processor.apvts.getParameter ("STRING")->setValueNotifyingHost (0.0f);

    auto buffer = makeTestBuffer (2, 512, 0.5f);
    juce::MidiBuffer midi;
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

TEST (PluginProcessorTest, StateRoundTripsThroughGetAndSetStateInformation)
{
    SonicTuningAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);

    auto* fineParam = processor.apvts.getParameter ("FINE");
    fineParam->setValueNotifyingHost (0.9f);
    float const savedFine = *processor.apvts.getRawParameterValue ("FINE");

    juce::MemoryBlock state;
    processor.getStateInformation (state);

    fineParam->setValueNotifyingHost (0.1f);
    ASSERT_NE (*processor.apvts.getRawParameterValue ("FINE"), savedFine);

    processor.setStateInformation (state.getData(), static_cast<int> (state.getSize()));

    EXPECT_FLOAT_EQ (*processor.apvts.getRawParameterValue ("FINE"), savedFine);
}
