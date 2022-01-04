/*
  ==============================================================================

    BpmInputFilter.cpp
    Created: 4 Jan 2022 2:19:11pm
    Author:  Andrew King

  ==============================================================================
*/

#include "BpmInputFilter.h"

juce::String BpmInputFilter::filterNewText(juce::TextEditor& e, const juce::String& newInput)
{
    juce::StringRef allowedChars("0123456789.");
    
    // return a copy of newInput, with only the allowed chars
    return newInput.retainCharacters(allowedChars);
}
