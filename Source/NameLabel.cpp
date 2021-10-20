#include "NameLabel.h"

NameLabel::NameLabel()
{
    setFont(20.f);
    setColour(juce::Label::textColourId, grey);
    setJustificationType(juce::Justification::centred);
}

NameLabel::~NameLabel()
{
    setLookAndFeel(nullptr);
}
