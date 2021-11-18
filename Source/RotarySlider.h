/*
  ==============================================================================

    RotarySlider.h
    Created: 18 Oct 2021 3:01:07pm
    Author:  Andrew King

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

class RotarySlider : public juce::Slider
{
public:
    RotarySlider();
    ~RotarySlider();
    
    void paint(juce::Graphics& g) override;
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    void setPathColor(juce::Colour color);
    
private:
    CustomLookAndFeel customLookAndFeel;
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour offWhite2 = juce::Colour::fromRGB(229, 229, 229);
    juce::Colour newGreen = juce::Colour::fromRGB(62, 218, 121);
    juce::Colour newRed = juce::Colour::fromRGB(249, 62, 59);
    juce::Colour newBlue = juce::Colour::fromRGB(13, 81, 230);
    juce::Colour newPink = juce::Colour::fromRGB(239, 59, 243);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotarySlider)
};
