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
    juce::Colour blue = juce::Colour::fromFloatRGBA(0.43f, 0.83f, 1.0f, 1.0f);
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour mint = juce::Colour::fromFloatRGBA(0.54f, 1.0f, 0.76f, 1.0f);
    juce::Colour fireRed = juce::Colour::fromFloatRGBA(0.93f, 0.38f, 0.33f, 1.0f);
    juce::Colour abkBlue = juce::Colour::fromRGB(5, 17, 242);
    juce::Colour abkRed = juce::Colour::fromRGB(242, 5, 5);
    juce::Colour abkGreen = juce::Colour::fromRGB(90, 157, 59);
    juce::Colour abkYellow = juce::Colour::fromRGB(233, 215, 88);
    juce::Colour abkPeach = juce::Colour::fromRGB(247, 178, 183);
    juce::Colour abkPink = juce::Colour::fromRGB(212, 17, 208);
    juce::Colour abkPurple = juce::Colour::fromRGB(61, 20, 245);
    
    juce::Colour offWhite2 = juce::Colour::fromRGB(229, 229, 229);
    juce::Colour abkLightGrey = juce::Colour::fromRGB(204, 204, 204);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel);
};
