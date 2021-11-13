/*
  ==============================================================================

    QueueModel.cpp
    Created: 11 Nov 2021 5:42:56pm
    Author:  Andrew King

  ==============================================================================
*/

#include "QueueModel.h"

int QueueModel::getNumRows()
{
    return (int)filePaths.size();
}

void QueueModel::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
//    g.fillAll(juce::Colours::dimgrey);
    
    if (rowIsSelected) {
        g.fillAll(offWhite);
    }
    // else if not selected: use alternating colors
    else if (rowNumber % 2 == 0) {
        g.fillAll(abkLightGrey);
    } else {
        g.fillAll(offWhite2);
    }
    
    juce::String someData = filePaths[rowNumber];
    g.setColour (juce::Colours::black);
    g.drawText (someData, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void QueueModel::addItem(std::string filePath)
{
    filePaths.push_back(filePath);
    return;
}

std::string QueueModel::popNext(int index)
{
    std::string temp = filePaths.at(index);
    filePaths.erase(filePaths.begin());
    return temp;
}
