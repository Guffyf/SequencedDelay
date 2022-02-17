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
    setSize (800, 400 + (50 * BasicDelayAudioProcessor::num_delays));

    for (int i = 0; i < BasicDelayAudioProcessor::num_delays; ++i)
    {
        auto numStr = std::to_string(i + 1);

        delay[i].setSliderStyle(juce::Slider::LinearBar);
        delay[i].setColour(juce::Slider::ColourIds::trackColourId, rainbow[i % 7]);
        delay[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        delay[i].setTextValueSuffix(" ms");
        addAndMakeVisible(&delay[i]);
        delayAttach[i].reset(new SliderAttachment(valueTreeState, "delay" + numStr, delay[i]));

        feedback[i].setSliderStyle(juce::Slider::LinearBar);
        feedback[i].setColour(juce::Slider::ColourIds::trackColourId, rainbow[i % 7]);
        feedback[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        feedback[i].setTextValueSuffix("%");
        addAndMakeVisible(&feedback[i]);
        feedbackAttach[i].reset(new SliderAttachment(valueTreeState, "fdbk" + numStr, feedback[i]));

        pan[i].setSliderStyle(juce::Slider::Rotary);
        pan[i].setColour(juce::Slider::ColourIds::thumbColourId, rainbow[i % 7].withAlpha(1.0f));
        pan[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 30);
        pan[i].setTextValueSuffix("%");
        addAndMakeVisible(&pan[i]);
        panAttach[i].reset(new SliderAttachment(valueTreeState, "pan" + numStr, pan[i]));
    }

    blend.setSliderStyle(juce::Slider::Rotary);
    blend.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 30);
    blend.setTextValueSuffix("%");
    blendAttach.reset(new SliderAttachment(valueTreeState, "blend", blend));
    addAndMakeVisible(blend);
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
    g.drawFittedText("Gabe Rook - 20220216", 0, 140, getWidth(), 30, juce::Justification::centred, 1);
}

void BasicDelayAudioProcessorEditor::resized()
{
    /// Called at initialization, and at resize if enabled
    // Set location of all components
    int i;
    for (i = 0; i < BasicDelayAudioProcessor::num_delays; ++i)
    {
        delay[i].setBounds(100, 200 + (i * 50), 295, 40);
        feedback[i].setBounds(405, 200 + (i * 50), 245, 40);
        pan[i].setBounds(650, 190 + (i * 50), 60, 60);
    }

    blend.setBounds(350, 200 + (i * 50), 100, 100);
}