#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include "PluginProcessor.h"

class MuffRotaryLook : public juce::LookAndFeel_V4
{
public:
    MuffRotaryLook();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

private:
    juce::Colour accentViolet {0xFF9B59B6};
    juce::Colour darkBg {0xFF1a1a1a};
};

class SonicMuffAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SonicMuffAudioProcessorEditor (SonicMuffAudioProcessor&);
    ~SonicMuffAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SonicMuffAudioProcessor& audioProcessor;

    MuffRotaryLook rotaryLook;

    juce::Slider sustainSlider, toneSlider, volumeSlider;
    juce::Label sustainLabel, toneLabel, volumeLabel;

    juce::ToggleButton bypassButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    void setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicMuffAudioProcessorEditor)
};
