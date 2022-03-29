#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SequencedDelay::~SequencedDelay()
{
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

void SequencedDelay::setCurrentProgram (int index)
{
}

const juce::String SequencedDelay::getProgramName (int index)
{
    return {};
}

void SequencedDelay::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SequencedDelay::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    for (int i = 0; i < num_delays; ++i)
    {
        delaySamples[i].reset(sampleRate, 0.00002f);
        gainSmooth[i].reset(sampleRate, 0.00002f);
    }
    blendSmooth.reset(sampleRate, 0.00002f);
    
    auto delayBufferSize = sampleRate * delay_buffer_length;
    delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));

    delayBuffer.clear();
    wetBuffer.clear();
}

void SequencedDelay::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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

    // Use global variable mainBuffer to provide access to buffer in helper methods
    mainBuffer = &buffer;

    bufferSize = mainBuffer->getNumSamples();
    delayBufferSize = delayBuffer.getNumSamples();

    // For mono inputs, copy left channel to right channel
    for (channel = inputChannels; channel < outputChannels; ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
        mainBuffer->copyFromWithRamp(channel, 0, mainBuffer->getReadPointer(channel % inputChannels, 0), bufferSize, 1.0f, 1.0f);
    }

    wetBuffer.setSize(getTotalNumOutputChannels(), bufferSize);

    // Loop handling writing of delays
    for (channel = 0; channel < outputChannels; ++channel)
    {
        loadDelayBuffer();

        for (size_t i = 0; i < num_delays; ++i)
        {
            writeDelay(i);
        }
    }

    writePosition += bufferSize;
    writePosition %= delayBufferSize;

    // Dry/wet mixing
    blendSmooth.setTargetValue(*blend);
    float wetGain = blendSmooth.getCurrentValue() / 100.0f;

    mainBuffer->applyGain(1.0f - wetGain);
    for (channel = 0; channel < outputChannels; ++channel)
    {
        mainBuffer->addFromWithRamp(channel, 0, wetBuffer.getReadPointer(channel, 0), bufferSize, wetGain, wetGain);
    }

    wetBuffer.clear();
}

// Loads the delayBuffer with new incoming information
void SequencedDelay::loadDelayBuffer()
{
    auto* channelData = mainBuffer->getWritePointer(channel);
    
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

void SequencedDelay::writeDelay(const size_t& delayNum)
{
    // Update gainSmooth and delaySamples
    gainSmooth[delayNum].setTargetValue(*gain[delayNum]);
    if (!(*sync[delayNum]))
    {
        delaySamples[delayNum].setTargetValue(getSampleRate() * (*delay[delayNum] / 1000.0f));
    }
    else
    {
        delaySamples[delayNum].setTargetValue(getSampleRate() * (60.0f / pos.bpm) * (*sixt[delayNum] / 4.0f));
    }

    writeDelay(delaySamples[delayNum].getCurrentValue(), (gainSmooth[delayNum].getCurrentValue() / 100.0f), (*pan[delayNum] / 100.0f));
}

// Reads from delayBuffer and copies to wetBuffer
// @param time - Delay time in samples
// @param gain - Delay gain
// @param pan - Delay pan
void SequencedDelay::writeDelay(float time, float gain, float pan)
{
    int readPosition = writePosition - time;

    if (readPosition < 0)
    {
        readPosition += delayBufferSize;
    }

    // Panning
    // https://forum.cockos.com/showthread.php?t=49809
    float channelGain;
    if (channel == 0)
    {
        channelGain = sin(0.5f * pi * (1.0f - pan)) * gain;
    }
    else
    {
        channelGain = sin(0.5f * pi * pan) * gain;
    }
    

    if (readPosition + bufferSize < delayBufferSize)
    {
        wetBuffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, channelGain, channelGain);
    }
    else
    {
        int numSamplesToEnd = delayBufferSize - readPosition;
        int numSamplesAtStart = bufferSize - numSamplesToEnd;

        wetBuffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, channelGain, channelGain);
        wetBuffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel), numSamplesAtStart, channelGain, channelGain);
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
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SequencedDelay();
}
