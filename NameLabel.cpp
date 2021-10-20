#include "NameLabel.h"

NameLabel::NameLabel()
{
    setFont(20.f);
    setColour(juce::Label::textColourId, offWhite);
    setJustificationType(juce::Justification::centred);
}

NameLabel::~NameLabel()
{
    setLookAndFeel(nullptr);
}
