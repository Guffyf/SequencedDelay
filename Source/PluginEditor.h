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

    juce::Slider blend;
    std::unique_ptr<SliderAttachment> blendAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDelayAudioProcessorEditor)
};
