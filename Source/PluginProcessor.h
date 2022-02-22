/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
const float pi = 2 * acos(0.0);
static constexpr int num_delays = 8;

//==============================================================================
class BasicDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (int i = 1; i <= num_delays; ++i)
        {
            auto numStr = std::to_string(i);
            layout.add(std::make_unique<juce::AudioParameterFloat>("delay" + numStr, "Delay " + numStr + " Time", 0.0f, 2000.0f, 250.0f));
            layout.add(std::make_unique<juce::AudioParameterFloat>("fdbk" + numStr, "Delay " + numStr + " Feedback", 0.0f, 100.0f, 0.0f));
            layout.add(std::make_unique<juce::AudioParameterFloat>("pan" + numStr, "Delay " + numStr + " Pan", 0.0f, 100.0f, 50.0f));
            layout.add(std::make_unique<juce::AudioParameterBool>("sync" + numStr, "Delay " + numStr + " Sync", false));
            layout.add(std::make_unique<juce::AudioParameterInt>("sixt" + numStr, "Delay " + numStr + " Sixteenths", 1, 16, 4));
        }

        layout.add(std::make_unique<juce::AudioParameterFloat>("blend", "Dry/Wet", 0.0f, 100.0f, 100.0f));

        return layout;
    }

    // https://docs.juce.com/master/tutorial_audio_bus_layouts.html
    // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    BasicDelayAudioProcessor() : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)), parameters(*this, nullptr, juce::Identifier("Main"), createParameterLayout())
    {
        for (int i = 0; i < num_delays; ++i)
        {
            auto numStr = std::to_string(i + 1);
            delay[i] = parameters.getRawParameterValue("delay" + numStr);
            fdbk[i] = parameters.getRawParameterValue("fdbk" + numStr);
            pan[i] = parameters.getRawParameterValue("pan" + numStr);
            sync[i] = parameters.getRawParameterValue("sync" + numStr);
            sixt[i] = parameters.getRawParameterValue("sixt" + numStr);
        }

        blend = parameters.getRawParameterValue("blend");
    }

    ~BasicDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void loadDelayBuffer();

    void writeDelay(float time, float gain, float pan, bool sync, int sixt);

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    inline bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* delay [num_delays] = { nullptr };
    std::atomic<float>* fdbk [num_delays] = { nullptr };
    std::atomic<float>* pan [num_delays] = { nullptr };
    std::atomic<float>* sync [num_delays] = { nullptr };
    std::atomic<float>* sixt [num_delays] = { nullptr };
    std::atomic<float>* blend = nullptr;
    //==============================================================================
    const float delay_buffer_length = 2.0f;
    //==============================================================================
    juce::AudioBuffer<float>* mainBuffer;
    int bufferSize{ 0 };

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize{ 0 };

    juce::AudioBuffer<float> wetBuffer;

    int channel{ 0 };
    int writePosition{ 0 };
    //==============================================================================
    juce::AudioPlayHead::CurrentPositionInfo pos;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDelayAudioProcessor)
};
