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
class customLook : public juce::LookAndFeel_V4
{
public:
    //==========================================================================
    customLook()
    {
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override
    {
        // https://docs.juce.com/master/tutorial_look_and_feel_customisation.html
        float radius = juce::jmin(width / 2, height / 2) - 1.0f;
        float ctrX = x + (width * 0.5f);
        float ctrY = y + (height * 0.5f);
        float radX = ctrX - radius;
        float radY = ctrY - radius;
        float dia = radius * 2.0;
        float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

        // Fill
        g.setColour(s.findColour(juce::Slider::ColourIds::thumbColourId));
        g.fillEllipse(radX, radY, dia, dia);
        // Outline
        g.setColour(juce::Colours::white);
        g.drawEllipse(radX, radY, dia, dia, 2.0f);
        // Pointer line
        juce::Path p;
        float thick = 2.0f;
        p.addRectangle(-thick * 0.5f, -radius, thick, radius);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(ctrX, ctrY));
        g.fillPath(p);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        if (!button.isEnabled())
            g.setOpacity(0.5f);

        if (button.getToggleState())
        {
            g.setColour(button.findColour(juce::ToggleButton::ColourIds::tickColourId));
            g.fillRect(0, 0, button.getWidth(), button.getHeight());
        }
        g.setColour(juce::Colours::white);
        g.drawRect(0, 0, button.getWidth(), button.getHeight(), 1);
    }
};

//==============================================================================
class BasicDelayAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    BasicDelayAudioProcessorEditor(BasicDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~BasicDelayAudioProcessorEditor() override;

    //==========================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==========================================================================
    void syncChanged();
    void selectChanged();

private:
    //==========================================================================
    customLook look;

    //==========================================================================
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::ComboBox select;

    juce::ToggleButton sync[num_delays];
    std::unique_ptr<ButtonAttachment> syncAttach[num_delays];
    juce::Slider delay[num_delays];
    std::unique_ptr<SliderAttachment> delayAttach[num_delays];
    juce::Slider sixt[num_delays];
    std::unique_ptr<SliderAttachment> sixtAttach[num_delays];
    juce::Slider gain[num_delays];
    std::unique_ptr<SliderAttachment> feedbackAttach[num_delays];
    juce::Slider pan[num_delays];
    std::unique_ptr<SliderAttachment> panAttach[num_delays];

    juce::Slider blend;
    std::unique_ptr<SliderAttachment> blendAttach;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDelayAudioProcessorEditor)
};