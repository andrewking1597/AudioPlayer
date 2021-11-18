/*
  ==============================================================================

    NameLabel.h
    Created: 18 Oct 2021 2:59:07pm
    Author:  Andrew King

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// This class is customized for the labels that correspond to each slider
// It prevents us from writing this code separately for each label

class NameLabel : public juce::Label
{
public:
//    NameLabel()
//    {
//        setFont(20.f);
//        setColour(juce::Label::textColourId, grey);
//        setJustificationType(juce::Justification::centred);
//    }
    NameLabel();
    
    ~NameLabel();
    
private:
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour offWhite2 = juce::Colour::fromRGB(229, 229, 229);
    juce::Colour newGreen = juce::Colour::fromRGB(62, 218, 121);
    juce::Colour newRed = juce::Colour::fromRGB(249, 62, 59);
    juce::Colour newBlue = juce::Colour::fromRGB(13, 81, 230);
    juce::Colour newPink = juce::Colour::fromRGB(239, 59, 243);
};
