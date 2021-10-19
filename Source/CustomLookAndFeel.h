/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 18 Oct 2021 2:56:14pm
    Author:  Andrew King

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel();
    
    // This method sets the basic placement and dimensions of the slider and its text box
    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;
    // This method controls the slider's appearance (color, arc size, etc)
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;
    // This method creates the slider's text box
    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    
private:
    // Define some custom colors
    juce::Colour blue = juce::Colour::fromFloatRGBA(0.43f, 0.83f, 1.0f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel);
};
