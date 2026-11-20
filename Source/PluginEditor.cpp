#include "PluginEditor.h"

ParticulaDubMatrixAudioProcessorEditor::ParticulaDubMatrixAudioProcessorEditor(
    ParticulaDubMatrixAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(760, 430);
    title.setText("PARTÍCULA DUB MATRIX", juce::dontSendNotification);
    title.setFont(juce::Font(25.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xffe8e6df));
    addAndMakeVisible(title);

    subtitle.setText("STEREO DELAY  /  FILTERED FEEDBACK PROCESSOR  /  ALPHA 0.1",
                     juce::dontSendNotification);
    subtitle.setFont(juce::Font(11.0f));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xff92958f));
    addAndMakeVisible(subtitle);

    setupSlider(delay, "TIME");
    setupSlider(feedback, "FEEDBACK");
    setupSlider(wet, "WET");
    setupSlider(lowpass, "LOW-PASS");
    setupSlider(highpass, "HIGH-PASS");
    setupSlider(drive, "DRIVE");

    auto& state = audioProcessor.parameters;
    delayAttachment = std::make_unique<SliderAttachment>(state, "delayMs", delay);
    feedbackAttachment = std::make_unique<SliderAttachment>(state, "feedback", feedback);
    wetAttachment = std::make_unique<SliderAttachment>(state, "mix", wet);
    lowpassAttachment = std::make_unique<SliderAttachment>(state, "lowpass", lowpass);
    highpassAttachment = std::make_unique<SliderAttachment>(state, "highpass", highpass);
    driveAttachment = std::make_unique<SliderAttachment>(state, "drive", drive);
}

void ParticulaDubMatrixAudioProcessorEditor::setupSlider(juce::Slider& s, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 22);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffc7a66a));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffeee8d9));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe8e6df));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff343a3a));
    s.setName(name);
    addAndMakeVisible(s);
}

void ParticulaDubMatrixAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111718));
    g.setColour(juce::Colour(0xff283333));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 8.0f, 1.0f);
    g.setColour(juce::Colour(0xffc7a66a));
    g.drawLine(24.0f, 83.0f, 736.0f, 83.0f, 1.0f);
    g.setColour(juce::Colour(0xff7f8984));
    g.setFont(10.0f);
    g.drawText("DELAY", 30, 105, 200, 16, juce::Justification::left);
    g.drawText("FEEDBACK CHARACTER", 270, 105, 220, 16, juce::Justification::left);
    g.drawText("OUTPUT", 520, 105, 200, 16, juce::Justification::left);
}

void ParticulaDubMatrixAudioProcessorEditor::resized()
{
    title.setBounds(28, 22, 500, 38);
    subtitle.setBounds(30, 58, 600, 20);

    const int y = 145, w = 112, h = 190;
    delay.setBounds(30, y, w, h);
    feedback.setBounds(150, y, w, h);
    lowpass.setBounds(270, y, w, h);
    highpass.setBounds(390, y, w, h);
    drive.setBounds(510, y, w, h);
    wet.setBounds(630, y, w, h);
}
