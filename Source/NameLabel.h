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
    
};
