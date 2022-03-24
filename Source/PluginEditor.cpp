#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
customLook::customLook()
{
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white);
}

void customLook::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s)
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

void customLook::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
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

// Modified version of juce::LookAndFeel_V4::drawLinerSlider
void customLook::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>(static_cast<float> (x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
            : juce::Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));
    }
    else
    {
        auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        /*
        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        juce::Path valueTrack;
        */
        juce::Point<float> minPoint, maxPoint, thumbPoint;
        

        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = { kx, ky };

        auto thumbWidth = getSliderThumbRadius(slider);
        /*
        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });
        */

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillRect(juce::Rectangle<float>(static_cast<float> (thumbWidth), height).withCentre(maxPoint));
    }
}

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

        time[i].setSliderStyle(juce::Slider::LinearHorizontal);
        time[i].setColour(juce::Slider::ColourIds::thumbColourId, colour.withAlpha(1.0f));
        time[i].setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        time[i].setLookAndFeel(&look);
        addAndMakeVisible(time[i]);

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
    g.drawFittedText("Gabe Rook - 20220324", 0, 140, getWidth(), 40, juce::Justification::centred, 1);

    g.setFont(12.0f);
    const int a = 300;
    g.drawFittedText("Sync", 100, a, 40, 20, juce::Justification::centred, 1);
    g.drawFittedText("Delay Time", 150, a, 245, 20, juce::Justification::centred, 1);
    g.drawFittedText("Gain", 405, a, 245, 20, juce::Justification::centred, 1);
    g.drawFittedText("Pan", 660, a, 40, 20, juce::Justification::centred, 1);
}

void BasicDelayAudioProcessorEditor::resized()
{
    /// Called at initialization, and at resize if enabled
    // Set location of all components
    const int a = 320;
    int i;
    for (i = 0; i < num_delays; ++i)
    {
        time[i].setBounds(100, 200, 600, 80);
        sync[i].setBounds(100, a, 40, 40);
        delay[i].setBounds(150, a, 245, 40);
        sixt[i].setBounds(150, a, 245, 40);
        gain[i].setBounds(405, a, 245, 40);
        pan[i].setBounds(660, a, 40, 40);
    }
    blend.setBounds(250, a + 50, 100, 100);
    select.setBounds(400, a + 50, 200, 40);
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