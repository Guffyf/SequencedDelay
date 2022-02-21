/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDelayAudioProcessor::~BasicDelayAudioProcessor()
{
}

//==============================================================================
const juce::String BasicDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicDelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = sampleRate * delay_buffer_length;
    delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));
}

void BasicDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool BasicDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    return true;
}

//==============================================================================
void BasicDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto inputChannels  = getTotalNumInputChannels();
    auto outputChannels = getTotalNumOutputChannels();

    // Use global variable mainBuffer to provide access to buffer in helper methods
    mainBuffer = &buffer;

    bufferSize = mainBuffer->getNumSamples();
    delayBufferSize = delayBuffer.getNumSamples();

    for (channel = inputChannels; channel < outputChannels; ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
        mainBuffer->copyFromWithRamp(channel, 0, mainBuffer->getReadPointer(channel % inputChannels, 0), bufferSize, 1.0f, 1.0f);
    }

    wetBuffer.setSize(getTotalNumOutputChannels(), bufferSize);

    for (channel = 0; channel < outputChannels; ++channel)
    {
        loadDelayBuffer();

        for (int i = 0; i < num_delays; ++i)
        {
            writeDelay(*delay[i], (*fdbk[i] / 100.0f), (*pan[i] / 100.0f));
        }
    }

    writePosition += bufferSize;
    writePosition %= delayBufferSize;

    float wetGain = *blend / 100.0f;

    mainBuffer->applyGain(1.0f - wetGain);
    for (channel = 0; channel < outputChannels; ++channel)
    {
        mainBuffer->addFromWithRamp(channel, 0, wetBuffer.getReadPointer(channel, 0), bufferSize, wetGain, wetGain);
    }

    wetBuffer.clear();
}

// Loads the delayBuffer with new incoming information
void BasicDelayAudioProcessor::loadDelayBuffer()
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

// Reads from delayBuffer and copies to wetBuffer
// @param delayTime - Delay time in ms
// @param delayGain - Delay gain in %
void BasicDelayAudioProcessor::writeDelay(float delayTime, float delayGain, float delayPan)
{
    int readPosition = writePosition - (getSampleRate() * (delayTime / 1000.0f));

    if (readPosition < 0)
    {
        readPosition += delayBufferSize;
    }

    float gain;
    if (channel == 0)
    {
        gain = sin(0.5f * pi * (1.0f - delayPan)) * delayGain;
    }
    else
    {
        gain = sin(0.5f * pi * delayPan) * delayGain;
    }
    

    if (readPosition + bufferSize < delayBufferSize)
    {
        wetBuffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, gain, gain);
    }
    else
    {
        int numSamplesToEnd = delayBufferSize - readPosition;
        int numSamplesAtStart = bufferSize - numSamplesToEnd;

        wetBuffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, gain, gain);
        wetBuffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, gain, gain);
    }
}

//==============================================================================
juce::AudioProcessorEditor* BasicDelayAudioProcessor::createEditor()
{
    return new BasicDelayAudioProcessorEditor (*this, parameters);
}

// Saves state information to a juce::MemoryBlock object for storage
void BasicDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Restores parameters from information saved using getStateInformation
void BasicDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new BasicDelayAudioProcessor();
}
