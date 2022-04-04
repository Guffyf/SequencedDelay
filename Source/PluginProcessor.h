#pragma once

#include <JuceHeader.h>

//==============================================================================
typedef std::shared_ptr<std::atomic<float>> sharedFloat;

//==============================================================================
const float pi = 2 * acos(0.0);
static constexpr int num_delays = 16;

//==============================================================================
class SequencedDelay : public juce::AudioProcessor
{
public:
    //==========================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (int i = 1; i <= num_delays; ++i)
        {
            auto numStr = std::to_string(i);
            layout.add(std::make_unique<juce::AudioParameterFloat>("delay" + numStr,
                "Delay " + numStr + " Time", 0.0f, 4000.0f, 250.0f));
            layout.add(std::make_unique<juce::AudioParameterFloat>("gain" + numStr,
                "Delay " + numStr + " Gain", 0.0f, 100.0f, 0.0f));
            layout.add(std::make_unique<juce::AudioParameterFloat>("pan" + numStr,
                "Delay " + numStr + " Pan", 0.0f, 100.0f, 50.0f));
            layout.add(std::make_unique<juce::AudioParameterBool>("sync" + numStr,
                "Delay " + numStr + " Sync", false));
            layout.add(std::make_unique<juce::AudioParameterInt>("sixt" + numStr,
                "Delay " + numStr + " Sixteenths", 1, 16, 4));
        }

        layout.add(std::make_unique<juce::AudioParameterFloat>("blend",
            "Dry/Wet", 0.0f, 100.0f, 100.0f));

        return layout;
    }

    // https://docs.juce.com/master/tutorial_audio_bus_layouts.html
    // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    SequencedDelay() :
        AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
        parameters(*this, nullptr, juce::Identifier("Main"), createParameterLayout()),
        viz(2)
    {
        for (int i = 0; i < num_delays; ++i)
        {
            delayResult[i] = std::make_shared<std::atomic<float>>(0.0f);

            auto numStr = std::to_string(i + 1);
            delay[i] = parameters.getRawParameterValue("delay" + numStr);
            gain[i] = parameters.getRawParameterValue("gain" + numStr);
            pan[i] = parameters.getRawParameterValue("pan" + numStr);
            sync[i] = parameters.getRawParameterValue("sync" + numStr);
            sixt[i] = parameters.getRawParameterValue("sixt" + numStr);
        }

        blend = parameters.getRawParameterValue("blend");
    }

    inline ~SequencedDelay() override {};

    //==========================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void loadDelayBuffer();
    void writeDelay(const size_t& delayNum);
    void writeDelay(juce::SmoothedValue<int>& time, juce::SmoothedValue<float>& gainL, juce::SmoothedValue<float>& gainR);

    //==========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    inline bool hasEditor() const override { return true; }

    //==========================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==========================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==========================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==========================================================================
    juce::AudioVisualiserComponent viz;
    sharedFloat delayResult [num_delays];

private:
    //==========================================================================
    juce::AudioBuffer<float>* mainBuffer;
    int bufferSize{ 0 };

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize{ 0 };

    juce::AudioBuffer<float> wetBuffer;

    int writePosition{ 0 };

    const float delay_buffer_length = 5.0f;

    //==========================================================================
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* sync [num_delays] = { nullptr };

    std::atomic<float>* delay [num_delays] = { nullptr };
    std::atomic<float>* sixt [num_delays] = { nullptr };
    juce::SmoothedValue<int> delaySamples [num_delays] = { 0 };

    std::atomic<float>* gain [num_delays] = { nullptr };
    std::atomic<float>* pan [num_delays] = { nullptr };
    juce::SmoothedValue<float> gainL[num_delays] = { 0.0f };
    juce::SmoothedValue<float> gainR[num_delays] = { 0.0f };
    
    std::atomic<float>* blend = nullptr;
    juce::SmoothedValue<float> blendSmooth = { 0.0f };
    
    //==========================================================================
    juce::AudioPlayHead::CurrentPositionInfo pos;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencedDelay)
};
