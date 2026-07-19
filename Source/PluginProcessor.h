#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <signalsmith-stretch/signalsmith-stretch.h>
#include "TuningTable.h"

class SonicTuningAudioProcessor  : public juce::AudioProcessor
{
public:
    SonicTuningAudioProcessor();
    ~SonicTuningAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) override { juce::ignoreUnused (this); }

    #if !ELK_HEADLESS
    juce::AudioProcessorEditor* createEditor() override;
    #endif
    bool hasEditor() const override;

    const juce::String getName() const override { return "SonicTuning"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    signalsmith::stretch::SignalsmithStretch<float> stretch;

    juce::AudioBuffer<float> wetBuffer;

    juce::LinearSmoothedValue<float> smoothedGain;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicTuningAudioProcessor)
};