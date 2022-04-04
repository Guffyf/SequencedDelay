//============================================================================//
//                                                                            //
//      Sequenced Delay - Gabe Rook                                           //
//                                                                            //
//============================================================================//

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
customLook::customLook()
{
    setColour(Slider::textBoxOutlineColourId, juce::Colours::white);
}

// Modified version of juce::LookAndFeel_V4::drawToggleButton
void customLook::drawToggleButton(Graphics& g, ToggleButton& button, 
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    if (!button.isEnabled())
        g.setOpacity(0.5f);

    g.setColour(button.findColour(
        ToggleButton::ColourIds::tickColourId).withAlpha(0.2f));
    g.fillRect(0, 0, button.getWidth(), button.getHeight());

    if (button.getToggleState())
    {
        g.setColour(button.findColour(
            ToggleButton::ColourIds::tickColourId).withAlpha(0.8f));
        g.fillRect(0, 0, button.getWidth(), button.getHeight());
    }

    g.setColour(button.findColour(
        ToggleButton::ColourIds::tickDisabledColourId));
    g.drawRect(0, 0, button.getWidth(), button.getHeight());
}

// Modified version of juce::LookAndFeel_V4::drawLinearSlider
void customLook::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        // juce_LookAndFeel_V2.cpp 1708 reduces bounds of the bar by 2
        sliderPos *= slider.isHorizontal() ? ((float)width + 2.0f) / (float)width
            : ((float)height + 2.0f) / (float)height;
        g.setColour(slider.findColour(Slider::trackColourId).withAlpha(0.80f));
        g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>
            (x - 1, y - 1, sliderPos - (float)x, height + 2)
            : juce::Rectangle<float>
            (x - 1, sliderPos, width + 2, y + (height - sliderPos)));
        g.setColour(slider.findColour(Slider::trackColourId).withAlpha(0.20f));
        g.fillRect(juce::Rectangle<float>(x - 1, y - 1, width + 2, height + 2));
    }
    else
    {
        juce::Point<float> startPoint(
            slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));
        juce::Point<float> endPoint(
            slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);
        juce::Point<float> minPoint, maxPoint, thumbPoint;
        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
        minPoint = startPoint;
        maxPoint = { kx, ky };
        auto thumbWidth = getSliderThumbRadius(slider) * 2;

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillRect(juce::Rectangle<float>
            (static_cast<float> (thumbWidth), height).withCentre(maxPoint));
    }
}

// Modified version of juce::LookAndFeel_V4::drawRotarySlider
void customLook::drawRotarySlider(Graphics& g, int x, int y, int width, int height, 
    float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, 
    Slider& s)
{
    // https://docs.juce.com/master/tutorial_look_and_feel_customisation.html
    float radius = juce::jmin(width / 2.0f, height / 2.0f) - 1.0f;
    float ctrX = x + (width * 0.5f);
    float ctrY = y + (height * 0.5f);
    float radX = ctrX - radius;
    float radY = ctrY - radius;
    float dia = radius * 2.0;
    float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

    // Fill
    g.setColour(s.findColour(juce::Slider::ColourIds::rotarySliderFillColourId));
    g.fillEllipse(radX, radY, dia, dia);
    // Outline
    g.setColour(s.findColour(juce::Slider::ColourIds::rotarySliderOutlineColourId));
    g.drawEllipse(radX, radY, dia, dia, 1.0f);
    // Pointer line
    g.setColour(juce::Colours::white);
    juce::Path p;
    float thick = 2.0f;
    p.addRectangle(-thick * 0.5f, -radius, thick, radius);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(ctrX, ctrY));
    g.fillPath(p);
}

// Modified version of juce::LookAndFeel_V4::drawComboBox
void customLook::drawComboBox(juce::Graphics& g, int width, int height,
    bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH,
    juce::ComboBox& box)
{
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRect(boxBounds.toFloat());

    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRect(boxBounds.toFloat(), 1.0f);

    juce::Rectangle<int> arrowZone(width - 30, 0, 20, height);
    juce::Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

    g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

//==============================================================================
mainSlider::mainSlider()
{
    setSliderStyle(Slider::LinearBar);
    setTextBoxStyle(Slider::TextBoxBelow, false, 200, 30);
    setColour(Slider::ColourIds::textBoxOutlineColourId,
        lineColour);
}

//==============================================================================
SequencedDelayEditor::SequencedDelayEditor
(SequencedDelay& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), valueTreeState(vts)
{
    setLookAndFeel(&look);

    setSize (800, 450);

    for (int i = 0; i < num_delays; ++i)
    {
        auto numStr = std::to_string(i + 1);
        auto colour = Colour(static_cast<float>(i / (num_delays - 1.0f)), 
            0.9f, 0.8f, 1.0f);

        sync[i].setColour(ToggleButton::ColourIds::tickColourId, colour);
        sync[i].setColour(ToggleButton::ColourIds::tickDisabledColourId, 
            lineColour);
        addAndMakeVisible(&sync[i]);
        syncAttach[i].reset(new ButtonAttachment
            (valueTreeState, "sync" + numStr, sync[i]));
        sync[i].onClick = [this] { syncChanged(); };

        delay[i].setColour(Slider::ColourIds::trackColourId, colour);
        delay[i].setTextValueSuffix(" ms");
        delay[i].setLookAndFeel(&look);
        addAndMakeVisible(&delay[i]);
        delayAttach[i].reset(new SliderAttachment
            (valueTreeState, "delay" + numStr, delay[i]));

        sixt[i].setColour(Slider::ColourIds::trackColourId, colour);
        sixt[i].setTextValueSuffix("/16");
        sixt[i].setLookAndFeel(&look);
        addAndMakeVisible(&sixt[i]);
        sixtAttach[i].reset(new SliderAttachment
            (valueTreeState, "sixt" + numStr, sixt[i]));

        gain[i].setColour(Slider::ColourIds::trackColourId, colour);
        gain[i].setTextValueSuffix("%");
        gain[i].setLookAndFeel(&look);
        addAndMakeVisible(&gain[i]);
        feedbackAttach[i].reset(new SliderAttachment
            (valueTreeState, "gain" + numStr, gain[i]));

        pan[i].setSliderStyle(Slider::Rotary);
        pan[i].setColour(Slider::ColourIds::rotarySliderFillColourId, colour);
        pan[i].setColour(Slider::ColourIds::rotarySliderOutlineColourId,
            lineColour);
        pan[i].setTextBoxStyle(Slider::NoTextBox, false, 60, 30);
        pan[i].setTextValueSuffix("%");
        addAndMakeVisible(&pan[i]);
        panAttach[i].reset(new SliderAttachment
            (valueTreeState, "pan" + numStr, pan[i]));

        select.addItem(numStr, i + 1);
    }

    select.setColour(ComboBox::ColourIds::backgroundColourId, darkColour);
    select.setColour(ComboBox::ColourIds::outlineColourId, lineColour);
    select.setScrollWheelEnabled(true);
    // https://docs.juce.com/master/tutorial_combo_box.html
    select.onChange = [this] { selectChanged(); };
    select.setSelectedId(1);
    selectChanged();
    addAndMakeVisible(&select);

    blend.setSliderStyle(Slider::Rotary);
    blend.setColour(Slider::ColourIds::rotarySliderFillColourId, darkColour);
    blend.setColour(Slider::ColourIds::rotarySliderOutlineColourId, lineColour);
    blend.setColour(Slider::ColourIds::textBoxOutlineColourId, 
        juce::Colours::white.withAlpha(0.0f));
    blend.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 30);
    blend.setTextValueSuffix("%");
    blendAttach.reset(new SliderAttachment(valueTreeState, "blend", blend));
    addAndMakeVisible(blend);
}

SequencedDelayEditor::~SequencedDelayEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SequencedDelayEditor::paint (juce::Graphics& g)
{
    // Fill whole window
    g.fillAll (Colour(0xff202020));

    // Set current drawing color
    g.setColour (juce::Colours::white);

    g.setFont (32.0f);
    g.drawFittedText("Sequenced Delay", 0, 60, getWidth(), 40, 
        juce::Justification::centred, 1);
    g.drawFittedText("Gabe Rook - 20220404", 0, 100, getWidth(), 40, 
        juce::Justification::centred, 1);

    g.setFont(12.0f);
    const int a = 150;
    g.drawFittedText("Sync", 100, a, 40, 20, 
        juce::Justification::centred, 1);
    g.drawFittedText("Delay Time", 150, a, 245, 20, 
        juce::Justification::centred, 1);
    g.drawFittedText("Gain", 405, a, 245, 20, 
        juce::Justification::centred, 1);
    g.drawFittedText("Pan", 660, a, 40, 20, 
        juce::Justification::centred, 1);
    g.drawFittedText("Mix", 350, a + 120, 100, 20,
        juce::Justification::centred, 1);

    // g.setColour(juce::Colours::black);

    // g.fillRect(juce::Rectangle<int>(0, 0, getWidth(), 40));
}

void SequencedDelayEditor::resized()
{
    /// Called at initialization, and at resize if enabled

    // Set location of all components
    const int a = 170;
    int i;
    for (i = 0; i < num_delays; ++i)
    {
        sync[i].setBounds(100, a, 40, 40);
        delay[i].setBounds(150, a, 245, 40);
        sixt[i].setBounds(150, a, 245, 40);
        gain[i].setBounds(405, a, 245, 40);
        pan[i].setBounds(660, a, 40, 40);
    }
    blend.setBounds(350, a + 120, 100, 100);
    select.setBounds(350, a + 50, 100, 40);
}

//==============================================================================
void SequencedDelayEditor::syncChanged()
{
    int i = select.getSelectedId() - 1;
    bool on = sync[i].getToggleState();

    delay[i].setVisible(!on);
    sixt[i].setVisible(on);
}

void SequencedDelayEditor::selectChanged()
{
    int t = select.getSelectedId() - 1;
    bool isSelected;

    for (int i = 0; i < num_delays; ++i)
    {
        isSelected = t == i;

        sync[i].setVisible(isSelected);
        delay[i].setVisible(isSelected && !sync[i].getToggleState());
        sixt[i].setVisible(isSelected && sync[i].getToggleState());
        gain[i].setVisible(isSelected);
        pan[i].setVisible(isSelected);
    }
}