/*
  ==============================================================================

    RotarySlider.cpp
    Created: 18 Oct 2021 3:01:20pm
    Author:  Andrew King

  ==============================================================================
*/

#include "RotarySlider.h"

RotarySlider::RotarySlider()
{
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0, 0);
    setLookAndFeel(&customLookAndFeel);
    setColour(juce::Slider::rotarySliderFillColourId, offWhite);
    setColour(juce::Slider::textBoxTextColourId, blackGrey);
    setColour(juce::Slider::textBoxOutlineColourId, grey);
    setVelocityBasedMode(true);
    setVelocityModeParameters(0.5, 1, 0.09, false);
    setRange(0.0, 100.0, 0.01);
    setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                        juce::MathConstants<float>::pi * 2.75f,
                        true);
    setWantsKeyboardFocus(true);
    setTextValueSuffix("%");
    
    // When the value is changed, update the number of
    // decimal places to display
    onValueChange = [&]()
    {
        if (getValue() < 10) {
            setNumDecimalPlacesToDisplay(2);
        } else if (getValue() >= 10 && getValue() < 100) {
            setNumDecimalPlacesToDisplay(1);
        } else {
            setNumDecimalPlacesToDisplay(0);
        }
    };
}

RotarySlider::~RotarySlider()
{
    setLookAndFeel(nullptr);
}

void RotarySlider::paint(juce::Graphics& g)
{
    juce::Slider::paint(g);
    
    if (hasKeyboardFocus(false)) {
        auto length = getHeight() > 15 ? 5.0f : 4.0f;
        auto thickness = getHeight() > 15 ? 3.0f : 2.5f;
        
        g.setColour(findColour(juce::Slider::textBoxOutlineColourId));
        
        //        fromX        fromY        toX                  toY
        g.drawLine(0,          0,           0,                   length,               thickness);
        g.drawLine(0,          0,           length,              0,                    thickness);
        g.drawLine(0,          getHeight(), 0,                   getHeight() - length, thickness);
        g.drawLine(0,          getHeight(), length,              getHeight(),          thickness);
        g.drawLine(getWidth(), getHeight(), getWidth() - length, getHeight(),          thickness);
        g.drawLine(getWidth(), getHeight(), getWidth(),          getHeight() - length, thickness);
        g.drawLine(getWidth(), 0,           getWidth() - length, 0,                    thickness);
        g.drawLine(getWidth(), 0,           getWidth(),          length,               thickness);
    }
}

// mouseDown() / mouseUp(): these functions tell the mouse to disappear when
// the user clicks+drags, and to reappear when released
void RotarySlider::mouseDown(const juce::MouseEvent& event)
{
    juce::Slider::mouseDown(event);
    setMouseCursor(juce::MouseCursor::NoCursor);
}
void RotarySlider::mouseUp(const juce::MouseEvent& event)
{
    juce::Slider::mouseUp(event);
    juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(event.source.getLastMouseDownPosition());
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

void RotarySlider::setPathColor(juce::Colour color)
{
    setColour(juce::Slider::rotarySliderFillColourId, color);
}
