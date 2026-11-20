#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ParticulaDubMatrixAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit ParticulaDubMatrixAudioProcessorEditor(ParticulaDubMatrixAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ParticulaDubMatrixAudioProcessor& audioProcessor;

    juce::Slider delay, feedback, wet, lowpass, highpass, drive;
    juce::Label title, subtitle;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> delayAttachment, feedbackAttachment, wetAttachment,
        lowpassAttachment, highpassAttachment, driveAttachment;

    void setupSlider(juce::Slider&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParticulaDubMatrixAudioProcessorEditor)
};
