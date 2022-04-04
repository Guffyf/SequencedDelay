//============================================================================//
//                                                                            //
//      Sequenced Delay - Gabe Rook                                           //
//                                                                            //
//============================================================================//

#pragma once

#include <JuceHeader.h>

using std::atomic;
using juce::SmoothedValue;

//==============================================================================
static constexpr int num_delays = 16;
const float delay_buffer_length = 5.0f;

const float pi = 2 * acos(0.0);

//==============================================================================
class SequencedDelay : public juce::AudioProcessor
{
public:
    //==========================================================================
    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (int i = 1; i <= num_delays; ++i)
        {
            auto numStr = std::to_string(i);
            layout.add(std::make_unique<juce::AudioParameterBool>
                ("sync" + numStr, "Delay " + numStr + " Sync", false));
            layout.add(std::make_unique<juce::AudioParameterFloat>
                ("delay" + numStr, "Delay " + numStr + " Time", 0.0f, 4000.0f, 250.0f));
            layout.add(std::make_unique<juce::AudioParameterInt>
                ("sixt" + numStr, "Delay " + numStr + " Sixteenths", 1, 16, 4));
            layout.add(std::make_unique<juce::AudioParameterFloat>
                ("gain" + numStr, "Delay " + numStr + " Gain", 0.0f, 100.0f, 0.0f));
            layout.add(std::make_unique<juce::AudioParameterFloat>
                ("pan" + numStr, "Delay " + numStr + " Pan", 0.0f, 100.0f, 50.0f));
        }

        layout.add(std::make_unique<juce::AudioParameterFloat>
            ("blend", "Dry/Wet", 0.0f, 100.0f, 100.0f));

        return layout;
    }

    // https://docs.juce.com/master/tutorial_audio_bus_layouts.html
    // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    SequencedDelay() :
        AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
        parameters(*this, nullptr, juce::Identifier("Main"), createParameterLayout())
    {
        for (int i = 0; i < num_delays; ++i)
        {
            auto numStr = std::to_string(i + 1);
            sync[i] = parameters.getRawParameterValue("sync" + numStr);
            delay[i] = parameters.getRawParameterValue("delay" + numStr);
            sixt[i] = parameters.getRawParameterValue("sixt" + numStr);
            gain[i] = parameters.getRawParameterValue("gain" + numStr);
            pan[i] = parameters.getRawParameterValue("pan" + numStr);
        }

        blend = parameters.getRawParameterValue("blend");
    }
    inline ~SequencedDelay() override {};

    //==========================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    inline void releaseResources() override {};

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    //==========================================================================
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void loadDelayBuffer();
    void writeDelay(const int& delayNum);
    void writeDelay(juce::SmoothedValue<int>& time, juce::SmoothedValue<float>& gainL, juce::SmoothedValue<float>& gainR);

    //==========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    inline bool hasEditor() const override { return true; }

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==========================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

private:
    //==========================================================================
    juce::AudioBuffer<float>* mainBuffer;
    int bufferSize{ 0 };

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize{ 0 };

    juce::AudioBuffer<float> wetBuffer;

    int writePosition{ 0 };

    //==========================================================================
    juce::AudioProcessorValueTreeState parameters;

    atomic<float>* sync [num_delays] = { nullptr };

    atomic<float>* delay [num_delays] = { nullptr };
    atomic<float>* sixt [num_delays] = { nullptr };
    SmoothedValue<int> delaySamples [num_delays] = { 0 };

    atomic<float>* gain [num_delays] = { nullptr };
    atomic<float>* pan [num_delays] = { nullptr };
    SmoothedValue<float> gainL [num_delays] = { 0.0f };
    SmoothedValue<float> gainR [num_delays] = { 0.0f };
    
    atomic<float>* blend = nullptr;
    SmoothedValue<float> blendSmooth = 0.0f;
    
    //==========================================================================
    juce::AudioPlayHead::CurrentPositionInfo pos;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencedDelay)
};
