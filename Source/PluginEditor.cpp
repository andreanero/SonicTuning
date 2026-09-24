#include "PluginEditor.h"

MuffRotaryLook::MuffRotaryLook()
{
    setColour (juce::Slider::rotarySliderFillColourId, accentViolet);
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xFF333333));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, darkBg);
    setColour (juce::Slider::textBoxTextColourId, accentViolet);
    setColour (juce::Slider::thumbColourId, accentViolet);
}

void MuffRotaryLook::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPosProportional, float rotaryStartAngle,
                                       float rotaryEndAngle, juce::Slider&)
{
    auto radius = (float) juce::jmin (width / 2, height / 2) - 2.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    g.setColour (juce::Colour (0xFF2a2a2a));
    g.fillEllipse (rx, ry, rw, rw);

    g.setColour (juce::Colour (0xFF444444));
    g.drawEllipse (rx, ry, rw, rw, 2.0f);

    juce::Path filledArc;
    filledArc.addCentredArc (centreX, centreY, radius, radius,
                            0.0f, rotaryStartAngle, angle, true);
    g.setColour (accentViolet);
    g.strokePath (filledArc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = 2.5f;
    pointer.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    pointer.applyTransform (juce::AffineTransform::rotation (angle, centreX, centreY));
    g.fillPath (pointer);
}

//==============================================================================

SonicMuffAudioProcessorEditor::SonicMuffAudioProcessorEditor (SonicMuffAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&rotaryLook);

    setupSlider (sustainSlider, sustainLabel, "SUSTAIN");
    setupSlider (toneSlider, toneLabel, "TONE");
    setupSlider (volumeSlider, volumeLabel, "VOLUME");

    sustainSlider.setRange (0.0f, 1.0f, 0.001f);
    toneSlider.setRange (0.0f, 1.0f, 0.001f);
    volumeSlider.setRange (0.0f, 1.0f, 0.001f);

    bypassButton.setButtonText ("BYPASS");
    bypassButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xFF9B59B6));
    bypassButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFF9B59B6));
    bypassButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xFF444444));
    addAndMakeVisible (bypassButton);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, "SUSTAIN", sustainSlider);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, "TONE", toneSlider);
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, "VOLUME", volumeSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, "BYPASS", bypassButton);

    setSize (420, 340);
}

SonicMuffAudioProcessorEditor::~SonicMuffAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SonicMuffAudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xFF1a1a1a));
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xFF9B59B6));
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    label.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    label.setColour (juce::Label::textColourId, juce::Colour (0xFF9B59B6));
}

void SonicMuffAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF0f0f0f));

    g.setColour (juce::Colour (0xFF222222));
    g.drawRect (getLocalBounds(), 2);

    g.setFont (juce::Font (juce::FontOptions (28.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFF9B59B6));
    g.drawFittedText ("SONICMUFF", getLocalBounds().removeFromTop (60), juce::Justification::centredTop, 1);

    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::italic)));
    g.setColour (juce::Colour (0xFF6E3B8C));
    g.drawFittedText ("FUZZ WITHOUT THE PEDAL", getLocalBounds().removeFromBottom (20), juce::Justification::centredBottom, 1);
}

void SonicMuffAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (70);

    auto knobRow = area.removeFromTop (160);
    auto knobWidth = knobRow.getWidth() / 3;
    sustainSlider.setBounds (knobRow.removeFromLeft (knobWidth).reduced (10));
    toneSlider.setBounds (knobRow.removeFromLeft (knobWidth).reduced (10));
    volumeSlider.setBounds (knobRow.removeFromLeft (knobWidth).reduced (10));

    area.removeFromTop (20);

    auto bypassRow = area.removeFromTop (30);
    bypassButton.setBounds (bypassRow.withSizeKeepingCentre (100, 30));
}
