/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDelayAudioProcessorEditor::BasicDelayAudioProcessorEditor
(BasicDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), valueTreeState(vts)
{
    setLookAndFeel(&look);

    setSize (800, 420 + (50 * num_delays));

    for (int i = 0; i < num_delays; ++i)
    {
        auto numStr = std::to_string(i + 1);
        auto colour = rainbow[i % 7];

        select.addItem(numStr, i + 1);

        sync[i].setColour(juce::ToggleButton::ColourIds::tickColourId, colour);
        addAndMakeVisible(&sync[i]);
        syncAttach[i].reset(new ButtonAttachment(valueTreeState, "sync" + numStr, sync[i]));
        sync[i].onClick = [this] { syncChanged(); };

        delay[i].setSliderStyle(juce::Slider::LinearBar);
        delay[i].setColour(juce::Slider::ColourIds::trackColourId, colour);
        delay[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        delay[i].setTextValueSuffix(" ms");
        delay[i].setLookAndFeel(&look);
        addAndMakeVisible(&delay[i]);
        delayAttach[i].reset(new SliderAttachment(valueTreeState, "delay" + numStr, delay[i]));

        sixt[i].setSliderStyle(juce::Slider::LinearBar);
        sixt[i].setColour(juce::Slider::ColourIds::trackColourId, colour);
        sixt[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        sixt[i].setTextValueSuffix("/16");
        sixt[i].setLookAndFeel(&look);
        addAndMakeVisible(&sixt[i]);
        sixtAttach[i].reset(new SliderAttachment(valueTreeState, "sixt" + numStr, sixt[i]));

        gain[i].setSliderStyle(juce::Slider::LinearBar);
        gain[i].setColour(juce::Slider::ColourIds::trackColourId, colour);
        gain[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 200, 30);
        gain[i].setTextValueSuffix("%");
        gain[i].setLookAndFeel(&look);
        addAndMakeVisible(&gain[i]);
        feedbackAttach[i].reset(new SliderAttachment(valueTreeState, "gain" + numStr, gain[i]));

        pan[i].setSliderStyle(juce::Slider::Rotary);
        pan[i].setColour(juce::Slider::ColourIds::thumbColourId, colour);
        pan[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 30);
        pan[i].setTextValueSuffix("%");
        addAndMakeVisible(&pan[i]);
        panAttach[i].reset(new SliderAttachment(valueTreeState, "pan" + numStr, pan[i]));
    }

    // https://docs.juce.com/master/tutorial_combo_box.html
    select.onChange = [this] { selectChanged(); };
    selectChanged();
    addAndMakeVisible(&select);

    blend.setSliderStyle(juce::Slider::Rotary);
    blend.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::black.withAlpha(0.5f));
    blend.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 30);
    blend.setTextValueSuffix("%");
    blendAttach.reset(new SliderAttachment(valueTreeState, "blend", blend));
    addAndMakeVisible(blend);
}

BasicDelayAudioProcessorEditor::~BasicDelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void BasicDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill whole window
    g.fillAll (juce::Colour(0xff1a1a1a));

    // Set current drawing color
    g.setColour (juce::Colours::white);

    g.setFont (32.0f);
    g.drawFittedText("Sequenced Delay", 0, 100, getWidth(), 40, juce::Justification::centred, 1);
    g.drawFittedText("Gabe Rook - 20220316", 0, 140, getWidth(), 40, juce::Justification::centred, 1);

    g.setFont(12.0f);
    g.drawFittedText("Sync", 100, 200, 40, 20, juce::Justification::centred, 1);
    g.drawFittedText("Delay Time", 150, 200, 245, 20, juce::Justification::centred, 1);
    g.drawFittedText("Gain", 405, 200, 245, 20, juce::Justification::centred, 1);
    g.drawFittedText("Pan", 660, 200, 40, 20, juce::Justification::centred, 1);
}

void BasicDelayAudioProcessorEditor::resized()
{
    /// Called at initialization, and at resize if enabled
    // Set location of all components
    int a = 220;
    int i, h;
    for (i = 0; i < num_delays; ++i)
    {
        h = i * 50;
        sync[i].setBounds(100, a, 40, 40);
        delay[i].setBounds(150, a, 245, 40);
        sixt[i].setBounds(150, a, 245, 40);
        gain[i].setBounds(405, a, 245, 40);
        pan[i].setBounds(660, a, 40, 40);
    }
    blend.setBounds(350, a + 50, 100, 100);
    select.setBounds(300, a + 200, 200, 40);
}

void BasicDelayAudioProcessorEditor::syncChanged()
{
    for (size_t i = 0; i < num_delays; ++i)
    {
        if (i == select.getSelectedId() - 1)
        {
            bool on = sync[i].getToggleState();
            delay[i].setVisible(!on);
            sixt[i].setVisible(on);
        }
    }
}

void BasicDelayAudioProcessorEditor::selectChanged()
{
    size_t t = select.getSelectedId() - 1;
    bool isSelected;

    for (size_t i = 0; i < num_delays; ++i)
    {
        isSelected = t == i;

        sync[i].setVisible(isSelected);
        delay[i].setVisible(isSelected && !sync[i].getToggleState());
        sixt[i].setVisible(isSelected && sync[i].getToggleState());
        gain[i].setVisible(isSelected);
        pan[i].setVisible(isSelected);
    }
}
