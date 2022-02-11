/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDelayAudioProcessorEditor::BasicDelayAudioProcessorEditor(BasicDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), valueTreeState(vts)
{
    setSize (800, 600);

    for (int i = 0; i < BasicDelayAudioProcessor::num_delays; ++i)
    {
        delay[i].setSliderStyle(juce::Slider::LinearBar);
        delay[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        delay[i].setTextValueSuffix(" ms");
        delay[i].setValue(100.0f);
        addAndMakeVisible(&delay[i]);
        delayAttach[i].reset(new SliderAttachment(valueTreeState, "delay" + std::to_string(i + 1), delay[i]));

        feedback[i].setSliderStyle(juce::Slider::LinearBar);
        feedback[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        feedback[i].setTextValueSuffix("%");
        feedback[i].setValue(0.0f);
        addAndMakeVisible(&feedback[i]);
        feedbackAttach[i].reset(new SliderAttachment(valueTreeState, "fdbk" + std::to_string(i + 1), feedback[i]));
    }
}

BasicDelayAudioProcessorEditor::~BasicDelayAudioProcessorEditor()
{
}

//==============================================================================
void BasicDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill whole window
    g.fillAll (juce::Colour(0xff0a0a0a));

    // Set current drawing color
    g.setColour (juce::Colours::white);

    g.setFont (24.0f);
    g.drawFittedText ("Sequenced Delay", 0, 100, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText("Gabe Rook - 20220302", 0, 140, getWidth(), 30, juce::Justification::centred, 1);
}

void BasicDelayAudioProcessorEditor::resized()
{
    /// Called at initialization, and at resize if enabled
    // Set location of all components
    for (int i = 0; i < BasicDelayAudioProcessor::num_delays; ++i)
    {
        delay[i].setBounds(100, 200 + (i * 50), 295, 40);
        feedback[i].setBounds(405, 200 + (i * 50), 295, 40);
    }
}