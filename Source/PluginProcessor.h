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
    BasicDelayAudioProcessor();
    ~BasicDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    /*
    Loads the delayBuffer with new incoming information
    */
    void loadDelayBuffer();

    /*
    Reads from delayBuffer and copies to wetBuffer

    @param delayTime    f
    */
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
    
    float delayMilliseconds[num_delays]{ 100.0f };
    float delayFeedback[num_delays]{ 0.0f };

    float blend{ 100.0f };
private:
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
