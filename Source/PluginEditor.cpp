#include "PluginEditor.h"

TuningRotaryLook::TuningRotaryLook()
{
    setColour (juce::Slider::rotarySliderFillColourId, accentAmber);
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xFF333333));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, darkBg);
    setColour (juce::Slider::textBoxTextColourId, accentAmber);
    setColour (juce::Slider::thumbColourId, accentAmber);
}

void TuningRotaryLook::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
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
    g.setColour (accentAmber);
    g.strokePath (filledArc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = 2.5f;
    pointer.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    pointer.applyTransform (juce::AffineTransform::rotation (angle, centreX, centreY));
    g.fillPath (pointer);
}

//==============================================================================

SonicTuningAudioProcessorEditor::SonicTuningAudioProcessorEditor (SonicTuningAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&rotaryLook);

    setupComboBox (tuningSelector, tuningLabel, "TUNING");
    setupComboBox (stringSelector, stringLabel, "STRING");

    tuningSelector.addItemList (tuningChoices(), 1);
    stringSelector.addItemList (stringChoices(), 1);

    setupSlider (fineSlider, fineLabel, "FINE");
    setupSlider (gainSlider, gainLabel, "GAIN");

    fineSlider.setRange (-50.0f, 50.0f, 0.1f);
    gainSlider.setRange (-24.0f, 24.0f, 0.1f);

    bypassButton.setButtonText ("BYPASS");
    bypassButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xFFFFA500));
    bypassButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFFFFA500));
    bypassButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xFF444444));
    addAndMakeVisible (bypassButton);

    tuningAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.apvts, "TUNING", tuningSelector);
    stringAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.apvts, "STRING", stringSelector);
    fineAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, "FINE", fineSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, "GAIN", gainSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, "BYPASS", bypassButton);

    setSize (500, 380);
}

SonicTuningAudioProcessorEditor::~SonicTuningAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SonicTuningAudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xFF1a1a1a));
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xFFFFA500));
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    label.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    label.setColour (juce::Label::textColourId, juce::Colour (0xFFFFA500));
}

void SonicTuningAudioProcessorEditor::setupComboBox (juce::ComboBox& box, juce::Label& label, const juce::String& name)
{
    box.setEditableText (false);
    box.setJustificationType (juce::Justification::centred);
    box.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFF2a2a2a));
    box.setColour (juce::ComboBox::textColourId, juce::Colour (0xFFFFA500));
    box.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF444444));
    box.setColour (juce::ComboBox::buttonColourId, juce::Colour (0xFFFFA500));
    addAndMakeVisible (box);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&box, false);
    label.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    label.setColour (juce::Label::textColourId, juce::Colour (0xFFFFA500));
}

void SonicTuningAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF0f0f0f));

    g.setColour (juce::Colour (0xFF222222));
    g.drawRect (getLocalBounds(), 2);

    g.setFont (juce::Font (juce::FontOptions (28.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFFFFA500));
    g.drawFittedText ("SONICTUNING", getLocalBounds().removeFromTop (60), juce::Justification::centredTop, 1);

    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::italic)));
    g.setColour (juce::Colour (0xFFAA7000));
    g.drawFittedText ("RETUNE WITHOUT RETUNING", getLocalBounds().removeFromBottom (20), juce::Justification::centredBottom, 1);
}

void SonicTuningAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (70);

    auto selectorRow = area.removeFromTop (60);
    tuningSelector.setBounds (selectorRow.removeFromBottom (30).reduced (40, 0));

    auto selectorRow2 = area.removeFromTop (60);
    stringSelector.setBounds (selectorRow2.removeFromBottom (30).reduced (40, 0));

    area.removeFromTop (20);

    auto knobRow = area.removeFromTop (140);
    auto knobWidth = knobRow.getWidth() / 2;
    fineSlider.setBounds (knobRow.removeFromLeft (knobWidth).reduced (10));
    gainSlider.setBounds (knobRow.removeFromLeft (knobWidth).reduced (10));

    auto bypassRow = area.removeFromTop (30);
    bypassButton.setBounds (bypassRow.withSizeKeepingCentre (100, 30));
}