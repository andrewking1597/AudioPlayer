/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 18 Oct 2021 2:56:45pm
    Author:  Andrew King

  ==============================================================================
*/

#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
}

CustomLookAndFeel::~CustomLookAndFeel()
{
}

juce::Slider::SliderLayout CustomLookAndFeel::getSliderLayout(juce::Slider& slider)
{
    auto localBounds = slider.getLocalBounds();
    
    juce::Slider::SliderLayout layout;
    
    layout.textBoxBounds = localBounds;
    layout.sliderBounds = localBounds;
    
    return layout;
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = radius * 0.1f;
    auto arcRadius = radius - lineW * 0.5f;
    
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                rotaryEndAngle,
                                true);

    g.setColour(blackGrey);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::beveled, juce::PathStrokeType::butt));

    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(),
                           bounds.getCentreY(),
                           arcRadius,
                           arcRadius,
                           0.0f,
                           rotaryStartAngle,
                           toAngle,
                           true);
    
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::beveled, juce::PathStrokeType::butt));
    
    auto alpha = 0.1f + (float) slider.getValue() * 0.9f / (float) slider.getMaximum();
    auto brightness = 0.4f + (float) slider.getValue() * 0.6f / (float) slider.getMaximum();

    g.setColour(fill.withAlpha(alpha).brighter(brightness));
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::beveled, juce::PathStrokeType::butt));

    auto thumbWidth = lineW * 2.0f;
 
    juce::Path pointer;
    pointer.addRectangle(-thumbWidth / 2, -thumbWidth / 2, thumbWidth, radius + lineW);
    
    g.setColour(offWhite);
    g.fillPath(pointer, juce::AffineTransform::rotation (toAngle + 3.12f).translated (bounds.getCentre()));

    g.fillEllipse(bounds.reduced (7.0f));
}

juce::Label* CustomLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto* lab = new juce::Label();
    lab->setJustificationType(juce::Justification::centred);
    lab->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    lab->setColour(juce::Label::textWhenEditingColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    lab->setColour(juce::Label::outlineWhenEditingColourId, slider.findColour(juce::Slider::textBoxOutlineColourId));
    lab->setInterceptsMouseClicks(false, false);
    lab->setFont(15);
    
    return lab;
}

juce::Font CustomLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    juce::Font font("Avenir Next Medium", 90.f, 0);
    return { font };
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
}
