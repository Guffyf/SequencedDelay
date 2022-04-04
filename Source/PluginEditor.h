//============================================================================//
//                                                                            //
//      Sequenced Delay - Gabe Rook                                           //
//                                                                            //
//============================================================================//

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h" 

using juce::Colour;
using juce::Graphics;
using juce::ToggleButton;
using juce::Slider;
using juce::ComboBox;

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
const Colour lineColour = juce::Colours::white.withAlpha(0.75f);
const Colour darkColour = juce::Colours::black.withAlpha(0.5f);

//==============================================================================
// LookAndFeel that overrides JUCE's default
class customLook : public juce::LookAndFeel_V4
{
public:
    //==========================================================================
    customLook();

    void drawToggleButton(Graphics& g, ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    inline int getSliderThumbRadius(Slider& slider) override { return 2; };

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos, 
        const Slider::SliderStyle style, Slider& slider);

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
        Slider& s) override;

    void drawComboBox(Graphics& g, int width, int height, 
        bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH,
        ComboBox& box) override;
};

//==============================================================================
class mainSlider : public Slider
{
public:
    mainSlider();
};

//==============================================================================
class SequencedDelayEditor : public juce::AudioProcessorEditor
{
public:
    SequencedDelayEditor(SequencedDelay& p,
        juce::AudioProcessorValueTreeState& vts);
    ~SequencedDelayEditor() override;

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

    ToggleButton sync[num_delays];
    std::unique_ptr<ButtonAttachment> syncAttach[num_delays];
    mainSlider delay[num_delays];
    std::unique_ptr<SliderAttachment> delayAttach[num_delays];
    mainSlider sixt[num_delays];
    std::unique_ptr<SliderAttachment> sixtAttach[num_delays];
    mainSlider gain[num_delays];
    std::unique_ptr<SliderAttachment> feedbackAttach[num_delays];
    Slider pan[num_delays];
    std::unique_ptr<SliderAttachment> panAttach[num_delays];

    Slider blend;
    std::unique_ptr<SliderAttachment> blendAttach;

    ComboBox select;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencedDelayEditor)
};