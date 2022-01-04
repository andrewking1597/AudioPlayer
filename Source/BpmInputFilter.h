/*
  ==============================================================================

    BpmInputFilter.h
    Created: 4 Jan 2022 2:18:20pm
    Author:  Andrew King

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <string>

class BpmInputFilter : public juce::TextEditor::InputFilter
{
public:
    juce::String filterNewText(juce::TextEditor& e, const juce::String& newInput);
};
