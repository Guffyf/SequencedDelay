//============================================================================//
//                                                                            //
//      Sequenced Delay - Gabe Rook                                           //
//                                                                            //
//============================================================================//

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
void SequencedDelay::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Setup juce::SmoothedValue objects
    for (int i = 0; i < num_delays; ++i)
    {
        delaySamples[i].reset(sampleRate, 0.2f);
        gainL[i].reset(sampleRate, 0.02f);
        gainR[i].reset(sampleRate, 0.02f);
    }
    blendSmooth.reset(sampleRate, 0.02f);
    
    // Setup delayBuffer and wetBuffer
    auto delayBufferSize = sampleRate * delay_buffer_length;
    delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));

    delayBuffer.clear();
    wetBuffer.clear();
}

bool SequencedDelay::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    return true;
}

//==============================================================================
void SequencedDelay::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto inputChannels  = getTotalNumInputChannels();
    auto outputChannels = getTotalNumOutputChannels();

    if (auto playhead = this->getPlayHead())
    {
        playhead->getCurrentPosition(pos);
    }

    // Use class variable mainBuffer to provide access to buffer in helper methods
    mainBuffer = &buffer;

    // For mono inputs, copy left channel to right channel
    for (int channel = inputChannels; channel < outputChannels; ++channel)
    {
        mainBuffer->clear(channel, 0, mainBuffer->getNumSamples());
        mainBuffer->copyFrom(channel, 0, mainBuffer->getReadPointer(channel % inputChannels, 0), bufferSize);
    }

    bufferSize = mainBuffer->getNumSamples();
    delayBufferSize = delayBuffer.getNumSamples();
    wetBuffer.setSize(getTotalNumOutputChannels(), bufferSize);

    loadDelayBuffer();

    for (int i = 0; i < num_delays; ++i)
    {
        writeDelay(i);
    }

    writePosition += bufferSize;
    writePosition %= delayBufferSize;

    // Dry/wet mixing
    blendSmooth.setTargetValue(*blend);
    float wetGain;

    // https://www.youtube.com/watch?v=HpGJH_gKRCU
    for (int sample = 0; sample < mainBuffer->getNumSamples(); ++sample)
    {
        wetGain = blendSmooth.getNextValue() / 100.0f;

        for (int channel = 0; channel < outputChannels; ++channel)
        {
            auto* dryData = mainBuffer->getWritePointer(channel, sample);
            auto* wetData = wetBuffer.getReadPointer(channel, sample);
            *dryData *= 1.0f - wetGain;
            *dryData += *wetData * wetGain;
        }
    }

    wetBuffer.clear();
}

// Loads the delayBuffer with new incoming information
void SequencedDelay::loadDelayBuffer()
{
    auto outputChannels = getTotalNumOutputChannels();
    
    for (int channel = 0; channel < outputChannels; ++channel)
    {
        auto* channelData = mainBuffer->getReadPointer(channel);

        // Check if main buffer copies to delay buffer without going out of bounds
        if (delayBufferSize > bufferSize + writePosition)
        {
            delayBuffer.copyFrom(channel, writePosition, channelData, bufferSize);
        }
        else
        {
            int numSamplesToEnd = delayBufferSize - writePosition;
            int numSamplesAtStart = bufferSize - numSamplesToEnd;

            // Copy from write to end
            delayBuffer.copyFrom(channel, writePosition, channelData, numSamplesToEnd);
            // Copy remaining samples from start onward
            delayBuffer.copyFrom(channel, 0, channelData + numSamplesToEnd, numSamplesAtStart);
        }
    }
}

// Calculates targets for smoothed values and calls other writeDelay
void SequencedDelay::writeDelay(const int& delayNum)
{
    // Update delaySamples
    if (!(*sync[delayNum]))
    {
        delaySamples[delayNum].setTargetValue(getSampleRate() * (*delay[delayNum] / 1000.0f));
    }
    else
    {
        delaySamples[delayNum].setTargetValue(getSampleRate() * (60.0f / pos.bpm) * (*sixt[delayNum] / 4.0f));
    }

    // Update gains
    auto thisPan = *pan[delayNum] / 100.0f;
    auto thisGain = *gain[delayNum] / 100.0f;
    // https://forum.cockos.com/showthread.php?t=49809
    gainL[delayNum].setTargetValue(sin(0.5f * pi * (1.0f - thisPan)) * thisGain);
    gainR[delayNum].setTargetValue(sin(0.5f * pi * thisPan) * thisGain);

    writeDelay(delaySamples[delayNum], gainL[delayNum], gainR[delayNum]);
}

// Writes delay to wetBuffer using SmoothedValue parameters
void SequencedDelay::writeDelay(juce::SmoothedValue<int>& time, juce::SmoothedValue<float>& gainL, juce::SmoothedValue<float>& gainR)
{
    auto outputChannels = getTotalNumOutputChannels();

    for (int sample = 0; sample < mainBuffer->getNumSamples(); ++sample)
    {
        // Find sample location in delayBuffer
        int pos = (writePosition + sample - time.getNextValue()
            + delayBufferSize) % delayBufferSize;

        for (int channel = 0; channel < outputChannels; ++channel)
        {
            auto* wetData = wetBuffer.getWritePointer(channel, sample);

            auto* bufferData = delayBuffer.getReadPointer(channel,
                pos);
            *wetData += *bufferData * (channel == 0 ?
                gainL.getNextValue() : gainR.getNextValue());
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* SequencedDelay::createEditor()
{
    return new SequencedDelayEditor (*this, parameters);
}

// Saves state information to a juce::MemoryBlock object for storage
void SequencedDelay::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Restores parameters from information saved using getStateInformation
void SequencedDelay::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml.get() != nullptr) 
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
const juce::String SequencedDelay::getName() const
{
    return JucePlugin_Name;
}

bool SequencedDelay::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SequencedDelay::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SequencedDelay::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SequencedDelay::getTailLengthSeconds() const
{
    return 0.0;
}

int SequencedDelay::getNumPrograms()
{
    return 1;
}

int SequencedDelay::getCurrentProgram()
{
    return 0;
}

void SequencedDelay::setCurrentProgram(int index)
{
}

const juce::String SequencedDelay::getProgramName(int index)
{
    return {};
}

void SequencedDelay::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SequencedDelay();
}