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
    setSize (800, 500);

    // Set parameters for juce::Slider delayA
    delayA.setSliderStyle(juce::Slider::LinearBar);
    delayA.setRange(0.0, 1000.0, 0.1);
    delayA.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
    delayA.setTextValueSuffix(" ms");
    delayA.setValue(100.0f);

    feedback.setSliderStyle(juce::Slider::LinearBar);
    feedback.setRange(0.0, 100.0, 0.1);
    feedback.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
    feedback.setTextValueSuffix("%");
    feedback.setValue(0.0f);

    addAndMakeVisible(&delayA);
    addAndMakeVisible(&feedback);

    delayA.addListener(this);
    feedback.addListener(this);
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
    g.drawFittedText ("Basic Delay", 0, 100, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText("Gabe Rook - 25012022", 0, 140, getWidth(), 30, juce::Justification::centred, 1);
}

void BasicDelayAudioProcessorEditor::resized()
{
    /// Called at initialization, and at resize if enabled
    // Set location of all components
    delayA.setBounds(100, 200, 600, 40);
    feedback.setBounds(100, 300, 600, 40);
}

void BasicDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.delayTime = delayA.getValue();
    audioProcessor.delayFeedback = feedback.getValue();
}