#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using juce::Colour;
using juce::uint8;
using juce::Graphics;
using juce::Slider;
using juce::ComboBox;
using juce::ToggleButton;

//==============================================================================
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

const Colour rainbow[7] = { Colour((uint8)255, (uint8)0, (uint8)0),
                            Colour((uint8)255, (uint8)127, (uint8)0),
                            Colour((uint8)255, (uint8)255, (uint8)0),
                            Colour((uint8)0, (uint8)255, (uint8)0),
                            Colour((uint8)0, (uint8)0, (uint8)255),
                            Colour((uint8)75, (uint8)0, (uint8)130),
                            Colour((uint8)148, (uint8)0, (uint8)211) };

//==============================================================================
class customLook : public juce::LookAndFeel_V4
{
public:
    //==========================================================================
    customLook();

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
        juce::Slider& s) override;

    void drawToggleButton(Graphics& g, ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    int getSliderThumbRadius(Slider& slider) override;

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos, 
        const juce::Slider::SliderStyle style, Slider& slider);

    void drawComboBox(Graphics& g, int width, int height, 
        bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH,
        ComboBox& box) override;
};

//==============================================================================
class timeDisplay : public Slider
{
public:
    //==========================================================================
    timeDisplay();

    inline void mouseMove(const juce::MouseEvent&) override {};

private:
    //==========================================================================
};

//==============================================================================
class SequencedDelayEditor : public juce::AudioProcessorEditor
{
public:
    SequencedDelayEditor(SequencedDelay& p,
        juce::AudioProcessorValueTreeState& vts);
    ~SequencedDelayEditor() override;

    //==========================================================================
    void paint(Graphics& g) override;
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

    juce::AudioVisualiserComponent* viz;
    timeDisplay time[num_delays];

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencedDelayEditor)
};