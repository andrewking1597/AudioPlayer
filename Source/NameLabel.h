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
    juce::Colour blue = juce::Colour::fromFloatRGBA(0.43f, 0.83f, 1.0f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour mint = juce::Colour::fromFloatRGBA(0.54f, 1.0f, 0.76f, 1.0f);
    juce::Colour fireRed = juce::Colour::fromFloatRGBA(0.93f, 0.38f, 0.33f, 1.0f);
};
