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
    setSize (800, 400 + (50 * num_delays));

    for (int i = 0; i < num_delays; ++i)
    {
        auto numStr = std::to_string(i + 1);
        auto colour = rainbow[i % 7];

        sync[i].setColour(juce::ToggleButton::ColourIds::tickColourId, colour);
        addAndMakeVisible(&sync[i]);
        syncAttach[i].reset(new ButtonAttachment(valueTreeState, "sync" + numStr, sync[i]));
        sync[i].addListener(this);

        delay[i].setSliderStyle(juce::Slider::LinearBar);
        delay[i].setColour(juce::Slider::ColourIds::trackColourId, colour);
        delay[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        delay[i].setTextValueSuffix(" ms");
        addAndMakeVisible(&delay[i]);
        delayAttach[i].reset(new SliderAttachment(valueTreeState, "delay" + numStr, delay[i]));

        sixt[i].setSliderStyle(juce::Slider::LinearBar);
        sixt[i].setColour(juce::Slider::ColourIds::trackColourId, colour);
        sixt[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        sixt[i].setTextValueSuffix("/16");
        addAndMakeVisible(&sixt[i]);
        sixtAttach[i].reset(new SliderAttachment(valueTreeState, "sixt" + numStr, sixt[i]));

        feedback[i].setSliderStyle(juce::Slider::LinearBar);
        feedback[i].setColour(juce::Slider::ColourIds::trackColourId, colour);
        feedback[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        feedback[i].setTextValueSuffix("%");
        addAndMakeVisible(&feedback[i]);
        feedbackAttach[i].reset(new SliderAttachment(valueTreeState, "fdbk" + numStr, feedback[i]));

        pan[i].setSliderStyle(juce::Slider::Rotary);
        pan[i].setColour(juce::Slider::ColourIds::thumbColourId, colour.withAlpha(1.0f));
        pan[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 30);
        pan[i].setTextValueSuffix("%");
        addAndMakeVisible(&pan[i]);
        panAttach[i].reset(new SliderAttachment(valueTreeState, "pan" + numStr, pan[i]));

        if (sync[i].getToggleState())
        {
            delay[i].setVisible(false);
        }
        else
        {
            sixt[i].setVisible(false);
        }
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
    g.drawFittedText("Gabe Rook - 20220221", 0, 140, getWidth(), 30, juce::Justification::centred, 1);
}

void BasicDelayAudioProcessorEditor::resized()
{
    /// Called at initialization, and at resize if enabled
    // Set location of all components
    int i;
    for (i = 0; i < num_delays; ++i)
    {
        auto h = i * 50;
        sync[i].setBounds(100, 200 + h, 40, 40);
        delay[i].setBounds(150, 200 + h, 245, 40);
        sixt[i].setBounds(150, 200 + h, 245, 40);
        feedback[i].setBounds(405, 200 + h, 245, 40);
        pan[i].setBounds(660, 200 + h, 40, 40);
    }

    blend.setBounds(350, 200 + (i * 50), 100, 100);
}

void BasicDelayAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    for (int i = 0; i < num_delays; ++i)
    {
        if (sync[i].getToggleState())
        {
            delay[i].setVisible(false);
            sixt[i].setVisible(true);
        }
        else
        {
            sixt[i].setVisible(false);
            delay[i].setVisible(true);
        }
    }
}
