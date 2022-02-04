/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDelayAudioProcessorEditor::BasicDelayAudioProcessorEditor (BasicDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);

    // Set parameters for juce::Slider delayA
    for (int i = 0; i < BasicDelayAudioProcessor::num_delays; ++i)
    {
        delay[i].setSliderStyle(juce::Slider::LinearBar);
        delay[i].setRange(0.0, 1000.0, 0.1);
        delay[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        delay[i].setTextValueSuffix(" ms");
        delay[i].setValue(100.0f);
        addAndMakeVisible(&delay[i]);
        delay[i].addListener(this);

        feedback[i].setSliderStyle(juce::Slider::LinearBar);
        feedback[i].setRange(0.0, 100.0, 0.1);
        feedback[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        feedback[i].setTextValueSuffix("%");
        feedback[i].setValue(0.0f);
        addAndMakeVisible(&feedback[i]);
        feedback[i].addListener(this);
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

void BasicDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    for (int i = 0; i < BasicDelayAudioProcessor::num_delays; ++i)
    {
        audioProcessor.delayMilliseconds[i] = delay[i].getValue();
        audioProcessor.delayFeedback[i] = feedback[i].getValue();
    }
}