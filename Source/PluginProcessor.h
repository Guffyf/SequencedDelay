/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class BasicDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    BasicDelayAudioProcessor()
        : parameters(*this, nullptr, juce::Identifier("Main"),
            // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
            // I cannot believe this
            {
                std::make_unique<juce::AudioParameterFloat> ("delay1",
                                                             "Delay 1",
                                                             0.0f,
                                                             2000.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("delay2",
                                                             "Delay 2",
                                                             0.0f,
                                                             2000.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("delay3",
                                                             "Delay 3",
                                                             0.0f,
                                                             2000.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("delay4",
                                                             "Delay 4",
                                                             0.0f,
                                                             2000.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("fdbk1",
                                                             "Feedback 1",
                                                             0.0f,
                                                             100.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("fdbk2",
                                                             "Feedback 2",
                                                             0.0f,
                                                             100.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("fdbk3",
                                                             "Feedback 3",
                                                             0.0f,
                                                             100.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("fdbk4",
                                                             "Feedback 4",
                                                             0.0f,
                                                             100.0f,
                                                             100.0f),
                std::make_unique<juce::AudioParameterFloat> ("blend",
                                                             "Dry/Wet",
                                                             0.0f,
                                                             100.0f,
                                                             100.0f),
            })
    #ifndef JucePlugin_PreferredChannelConfigurations : AudioProcessor(BusesProperties())
    #endif
    {
        for (int i = 0; i < num_delays; ++i)
        {
            delay[i] = parameters.getRawParameterValue("delay" + std::to_string(i + 1));
            fdbk[i] = parameters.getRawParameterValue("fdbk" + std::to_string(i + 1));
        }
        blend = parameters.getRawParameterValue("blend");
    }
    ~BasicDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void loadDelayBuffer();

    void writeDelay(float delayTime, float delayGain);

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

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

    //==============================================================================
    static constexpr int num_delays = 4;
    
    
private:
    //==============================================================================
    // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* delay [num_delays] = { nullptr };
    std::atomic<float>* fdbk[num_delays] = { nullptr };
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDelayAudioProcessor)
};
