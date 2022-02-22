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
class BasicDelayAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       public juce::Button::Listener
{
public:
    BasicDelayAudioProcessorEditor(BasicDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~BasicDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked(juce::Button* button) override;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider delay[num_delays];
    std::unique_ptr<SliderAttachment> delayAttach[num_delays];

    juce::Slider feedback[num_delays];
    std::unique_ptr<SliderAttachment> feedbackAttach[num_delays];

    juce::Slider pan[num_delays];
    std::unique_ptr<SliderAttachment> panAttach[num_delays];

    juce::ToggleButton sync[num_delays];
    std::unique_ptr<ButtonAttachment> syncAttach[num_delays];

    juce::Slider sixt[num_delays];
    std::unique_ptr<SliderAttachment> sixtAttach[num_delays];

    juce::Slider blend;
    std::unique_ptr<SliderAttachment> blendAttach;
    //==============================================================================

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDelayAudioProcessorEditor)
};