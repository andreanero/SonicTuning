#include "PluginProcessor.h"
#if !ELK_HEADLESS
#include "PluginEditor.h"
#endif

SonicTuningAudioProcessor::SonicTuningAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

SonicTuningAudioProcessor::~SonicTuningAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout SonicTuningAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "TUNING", "Target Tuning", tuningChoices(), 0));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "STRING", "String", stringChoices(), 0));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "FINE", "Fine Tune", juce::NormalisableRange<float> (-50.0f, 50.0f, 0.1f), 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "GAIN", "Output Gain", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        "BYPASS", "Bypass", true));

    return layout;
}

void SonicTuningAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    int const numChannels = getTotalNumInputChannels();

    stretch.presetDefault (numChannels, static_cast<float> (sampleRate));
    setLatencySamples (stretch.inputLatency() + stretch.outputLatency());

    wetBuffer.setSize (numChannels, samplesPerBlock, false, false, true);

    smoothedGain.reset (sampleRate, 0.02);
}

void SonicTuningAudioProcessor::releaseResources()
{
    stretch.reset();
}

void SonicTuningAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (*apvts.getRawParameterValue ("BYPASS") > 0.5f)
        return;

    int const tuningIndex = static_cast<int> (*apvts.getRawParameterValue ("TUNING"));
    int const stringIndex = static_cast<int> (*apvts.getRawParameterValue ("STRING"));
    float const fineCents = *apvts.getRawParameterValue ("FINE");
    float const gainDb = *apvts.getRawParameterValue ("GAIN");

    double const semitones = semitoneShiftForString (tuningIndex, stringIndex) + fineCents / 100.0;
    stretch.setTransposeSemitones (static_cast<float> (semitones));

    smoothedGain.setTargetValue (juce::Decibels::decibelsToGain (gainDb));

    int const numSamples = buffer.getNumSamples();
    int const numChannels = buffer.getNumChannels();

    stretch.process (buffer.getArrayOfReadPointers(), numSamples, wetBuffer.getArrayOfWritePointers(), numSamples);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float const gain = smoothedGain.getNextValue();
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.setSample (ch, sample, wetBuffer.getSample (ch, sample) * gain);
    }
}

void SonicTuningAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SonicTuningAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));

    apvts.getParameter ("BYPASS")->setValueNotifyingHost (1.0f);
}

bool SonicTuningAudioProcessor::hasEditor() const
{
    #if ELK_HEADLESS
    return false;
    #else
    return true;
    #endif
}

#if !ELK_HEADLESS
juce::AudioProcessorEditor* SonicTuningAudioProcessor::createEditor()
{
    return new SonicTuningAudioProcessorEditor (*this);
}
#endif

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SonicTuningAudioProcessor();
}