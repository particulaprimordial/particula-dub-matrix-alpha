#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
constexpr float pi = 3.14159265358979323846f;
}

ParticulaDubMatrixAudioProcessor::ParticulaDubMatrixAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

ParticulaDubMatrixAudioProcessor::APVTS::ParameterLayout
ParticulaDubMatrixAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayMs", "Delay Time", juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.45f), 420.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "feedback", "Feedback", juce::NormalisableRange<float>(0.0f, 0.92f, 0.001f), 0.58f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Wet", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.35f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lowpass", "Low-pass", juce::NormalisableRange<float>(300.0f, 18000.0f, 1.0f, 0.35f), 6500.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "highpass", "High-pass", juce::NormalisableRange<float>(20.0f, 1500.0f, 1.0f, 0.45f), 90.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "drive", "Drive", juce::NormalisableRange<float>(0.0f, 12.0f, 0.01f), 1.5f));
    return { p.begin(), p.end() };
}

void ParticulaDubMatrixAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    maxDelaySamples = static_cast<int>(std::ceil(sampleRate * 2.1)) + 4;
    for (auto& c : channels)
    {
        c.delay.assign(static_cast<size_t>(maxDelaySamples), 0.0f);
        c.writePosition = 0;
        c.lowState = c.highInputState = c.highOutputState = 0.0f;
    }
}

void ParticulaDubMatrixAudioProcessor::releaseResources()
{
    resetState();
}

void ParticulaDubMatrixAudioProcessor::resetState() noexcept
{
    for (auto& c : channels)
    {
        std::fill(c.delay.begin(), c.delay.end(), 0.0f);
        c.writePosition = 0;
        c.lowState = c.highInputState = c.highOutputState = 0.0f;
    }
}

float ParticulaDubMatrixAudioProcessor::readInterpolated(const ChannelState& c, float delaySamples) const noexcept
{
    if (c.delay.empty()) return 0.0f;
    const auto size = static_cast<int>(c.delay.size());
    const float bounded = juce::jlimit(1.0f, static_cast<float>(size - 2), delaySamples);
    const float readPosition = static_cast<float>(c.writePosition) - bounded;
    const float wrapped = readPosition < 0.0f ? readPosition + static_cast<float>(size) : readPosition;
    const int i0 = static_cast<int>(wrapped) % size;
    const int i1 = (i0 + 1) % size;
    const float frac = wrapped - std::floor(wrapped);
    return c.delay[static_cast<size_t>(i0)] * (1.0f - frac)
         + c.delay[static_cast<size_t>(i1)] * frac;
}

void ParticulaDubMatrixAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                     juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin(2, buffer.getNumChannels());
    if (maxDelaySamples <= 0 || numChannels == 0) return;

    const float delayMs = parameters.getRawParameterValue("delayMs")->load();
    const float feedback = parameters.getRawParameterValue("feedback")->load();
    const float wet = parameters.getRawParameterValue("mix")->load();
    const float lpHz = parameters.getRawParameterValue("lowpass")->load();
    const float hpHz = parameters.getRawParameterValue("highpass")->load();
    const float drive = parameters.getRawParameterValue("drive")->load();

    const float delaySamples = juce::jlimit(1.0f, static_cast<float>(maxDelaySamples - 2),
        delayMs * 0.001f * static_cast<float>(currentSampleRate));
    const float lpA = 1.0f - std::exp(-2.0f * pi * lpHz / static_cast<float>(currentSampleRate));
    const float hpA = std::exp(-2.0f * pi * hpHz / static_cast<float>(currentSampleRate));
    const float driveGain = juce::Decibels::decibelsToGain(drive);
    const float dry = 1.0f - wet;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channels[static_cast<size_t>(ch)];

        for (int n = 0; n < numSamples; ++n)
        {
            const float input = data[n];
            const float delayed = readInterpolated(state, delaySamples);

            // One-pole low-pass followed by a one-pole high-pass in the feedback path.
            state.lowState += lpA * (delayed - state.lowState);
            const float hp = hpA * (state.highOutputState + state.lowState - state.highInputState);
            state.highInputState = state.lowState;
            state.highOutputState = hp;

            const float shaped = std::tanh(hp * driveGain);
            const float feedbackSample = shaped * feedback;

            state.delay[state.writePosition] = input + feedbackSample;
            state.writePosition = (state.writePosition + 1) % state.delay.size();

            data[n] = dry * input + wet * delayed;
        }
    }

    // For mono input/output configurations, duplicate-safe handling is delegated to the host bus layout.
}

juce::AudioProcessorEditor* ParticulaDubMatrixAudioProcessor::createEditor()
{
    return new ParticulaDubMatrixAudioProcessorEditor(*this);
}

void ParticulaDubMatrixAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ParticulaDubMatrixAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParticulaDubMatrixAudioProcessor();
}
