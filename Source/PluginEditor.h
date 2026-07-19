#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include "PluginProcessor.h"

class TuningRotaryLook : public juce::LookAndFeel_V4
{
public:
    TuningRotaryLook();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

private:
    juce::Colour accentAmber {0xFFFFA500};
    juce::Colour darkBg {0xFF1a1a1a};
};

class SonicTuningAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SonicTuningAudioProcessorEditor (SonicTuningAudioProcessor&);
    ~SonicTuningAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SonicTuningAudioProcessor& audioProcessor;

    TuningRotaryLook rotaryLook;

    juce::ComboBox tuningSelector, stringSelector;
    juce::Label tuningLabel, stringLabel;

    juce::Slider fineSlider, gainSlider;
    juce::Label fineLabel, gainLabel;

    juce::ToggleButton bypassButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> tuningAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> stringAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fineAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    void setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name);
    void setupComboBox (juce::ComboBox& box, juce::Label& label, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicTuningAudioProcessorEditor)
};