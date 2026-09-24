#include "PluginProcessor.h"
#if !ELK_HEADLESS
#include "PluginEditor.h"
#endif
#include <array>

SonicMuffAudioProcessor::SonicMuffAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

SonicMuffAudioProcessor::~SonicMuffAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout SonicMuffAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "SUSTAIN", "Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "TONE", "Tone", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "VOLUME", "Volume", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        "BYPASS", "Bypass", true));

    return layout;
}

void SonicMuffAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    currentSampleRate = sampleRate;

    stageOneHpCoeff = BigMuff::onePoleCoefficient (BigMuff::kStageOneHighPassHz, sampleRate);
    stageTwoHpCoeff = BigMuff::onePoleCoefficient (BigMuff::kStageTwoHighPassHz, sampleRate);
    toneBassCoeff   = BigMuff::onePoleCoefficient (BigMuff::kToneBassHz, sampleRate);
    toneTrebleCoeff = BigMuff::onePoleCoefficient (BigMuff::kToneTrebleHz, sampleRate);

    channelStates.assign (static_cast<size_t> (juce::jmax (1, getTotalNumInputChannels())), BigMuff::ChannelState {});
    for (auto& state : channelStates)
        state.reset();

    smoothedSustain.reset (sampleRate, 0.02);
    smoothedTone.reset    (sampleRate, 0.02);
    smoothedVolume.reset  (sampleRate, 0.02);
}

void SonicMuffAudioProcessor::releaseResources()
{
    for (auto& state : channelStates)
        state.reset();
}

void SonicMuffAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (*apvts.getRawParameterValue ("BYPASS") > 0.5f)
        return;

    smoothedSustain.setTargetValue (*apvts.getRawParameterValue ("SUSTAIN"));
    smoothedTone.setTargetValue    (*apvts.getRawParameterValue ("TONE"));
    smoothedVolume.setTargetValue  (*apvts.getRawParameterValue ("VOLUME"));

    int const numSamples = buffer.getNumSamples();
    int const numChannels = juce::jmin (buffer.getNumChannels(), static_cast<int> (channelStates.size()));

    std::array<float*, 2> channelPtrs {};
    for (int channel = 0; channel < numChannels; ++channel)
        channelPtrs[static_cast<size_t> (channel)] = buffer.getWritePointer (channel);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float const sustain = smoothedSustain.getNextValue();
        float const tone    = smoothedTone.getNextValue();
        float const volume  = smoothedVolume.getNextValue();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = channelPtrs[static_cast<size_t> (channel)];
            float const shaped = BigMuff::process (channelData[sample], channelStates[static_cast<size_t> (channel)],
                                                    stageOneHpCoeff, stageTwoHpCoeff,
                                                    toneBassCoeff, toneTrebleCoeff,
                                                    sustain, tone);
            channelData[sample] = shaped * volume;
        }
    }
}

void SonicMuffAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SonicMuffAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));

    apvts.getParameter ("BYPASS")->setValueNotifyingHost (1.0f);
}

bool SonicMuffAudioProcessor::hasEditor() const
{
    #if ELK_HEADLESS
    return false;
    #else
    return true;
    #endif
}

#if !ELK_HEADLESS
juce::AudioProcessorEditor* SonicMuffAudioProcessor::createEditor()
{
    return new SonicMuffAudioProcessorEditor (*this);
}
#endif

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SonicMuffAudioProcessor();
}
