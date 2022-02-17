/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::uint8 int8;

const juce::Colour rainbow[7] = { juce::Colour((int8)255, (int8)0, (int8)0, 0.5f),
                                  juce::Colour((int8)255, (int8)127, (int8)0, 0.5f),
                                  juce::Colour((int8)255, (int8)255, (int8)0, 0.5f),
                                  juce::Colour((int8)0, (int8)255, (int8)0, 0.5f),
                                  juce::Colour((int8)0, (int8)0, (int8)255, 0.5f),
                                  juce::Colour((int8)75, (int8)0, (int8)130, 0.5f),
                                  juce::Colour((int8)148, (int8)0, (int8)211, 0.5f) };

//==============================================================================
class BasicDelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicDelayAudioProcessorEditor(BasicDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~BasicDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider delay[BasicDelayAudioProcessor::num_delays];
    std::unique_ptr<SliderAttachment> delayAttach[BasicDelayAudioProcessor::num_delays];

    juce::Slider feedback[BasicDelayAudioProcessor::num_delays];
    std::unique_ptr<SliderAttachment> feedbackAttach[BasicDelayAudioProcessor::num_delays];

    juce::Slider pan[BasicDelayAudioProcessor::num_delays];
    std::unique_ptr<SliderAttachment> panAttach[BasicDelayAudioProcessor::num_delays];

    juce::Slider blend;
    std::unique_ptr<SliderAttachment> blendAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDelayAudioProcessorEditor)
};