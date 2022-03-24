#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h" 

//==============================================================================
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::Colour Colour;
typedef juce::uint8 int8;

const Colour rainbow[7] = { Colour((int8)255, (int8)0, (int8)0, 0.5f),
                            Colour((int8)255, (int8)127, (int8)0, 0.5f),
                            Colour((int8)255, (int8)255, (int8)0, 0.5f),
                            Colour((int8)0, (int8)255, (int8)0, 0.5f),
                            Colour((int8)0, (int8)0, (int8)255, 0.5f),
                            Colour((int8)75, (int8)0, (int8)130, 0.5f),
                            Colour((int8)148, (int8)0, (int8)211, 0.5f) };

//==============================================================================
class customLook : public juce::LookAndFeel_V4
{
public:
    //==========================================================================
    customLook();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider);
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

    juce::Slider time[num_delays];

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