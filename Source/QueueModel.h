/*
  ==============================================================================

    QueueModel.h
    Created: 11 Nov 2021 5:42:39pm
    Author:  Andrew King

  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

class QueueModel : public juce::ListBoxModel
{
public:
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void addItem(juce::File file);
    void addItem(juce::String absolutePath);
    juce::File popHead();
    juce::File getHead();
    juce::File* getHeadPtr();
    void deleteRow(int rowNumber);
private:
    std::vector<juce::File> files;
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour offWhite2 = juce::Colour::fromRGB(229, 229, 229);
    juce::Colour newGreen = juce::Colour::fromRGB(62, 218, 121);
    juce::Colour newRed = juce::Colour::fromRGB(249, 62, 59);
    juce::Colour newBlue = juce::Colour::fromRGB(13, 81, 230);
    juce::Colour newPink = juce::Colour::fromRGB(239, 59, 243);
};
