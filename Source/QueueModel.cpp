/*
  ==============================================================================

    QueueModel.cpp
    Created: 11 Nov 2021 5:42:56pm
    Author:  Andrew King

  ==============================================================================
*/

#include "QueueModel.h"

int QueueModel::getNumRows() {
    return (int)files.size();
}

void QueueModel::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) {
        g.fillAll(offWhite2);
    } else {
        g.fillAll(offWhite);
    }
    
    juce::String fname = files[rowNumber].getFileNameWithoutExtension();
    g.setColour (juce::Colours::black);
    g.drawText (fname, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void QueueModel::addItem(juce::File file) {
    files.push_back(file);
    return;
}

void QueueModel::addItem(juce::String absolutePath) {
    juce::File temp(absolutePath);
    files.push_back(temp);
    return;
}

juce::File QueueModel::popHead() {
    juce::File temp = files.at(0);
    files.erase(files.begin());
    return temp;
}

juce::File QueueModel::getHead() {
    return files.at(0);
}

juce::File* QueueModel::getHeadPtr() {
    return &files.at(0);
}

void QueueModel::deleteRow(int rowNumber)
{
    // delete from files vector
    files.erase(files.begin()+rowNumber);
    return;
}
