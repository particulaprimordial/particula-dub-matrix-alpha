#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>

class ParticulaDubMatrixAudioProcessor final : public juce::AudioProcessor
{
public:
    ParticulaDubMatrixAudioProcessor();
    ~ParticulaDubMatrixAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 20.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS parameters;

    static APVTS::ParameterLayout createParameterLayout();

private:
    struct ChannelState
    {
        std::vector<float> delay;
        size_t writePosition = 0;
        float lowState = 0.0f;
        float highInputState = 0.0f;
        float highOutputState = 0.0f;
    };

    std::array<ChannelState, 2> channels;
    double currentSampleRate = 44100.0;
    int maxDelaySamples = 0;

    float readInterpolated(const ChannelState&, float delaySamples) const noexcept;
    void resetState() noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParticulaDubMatrixAudioProcessor)
};
