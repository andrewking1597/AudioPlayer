#pragma once

#include <JuceHeader.h>

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    
    TransportState state; // Keeps track of the state of audio playback
    juce::AudioFormatManager formatManager; // Controls what audio formats are allowed (.wav and .aiff)
    std::unique_ptr<juce::AudioFormatReaderSource> playSource; // plays data received from tempSource
    juce::AudioTransportSource transport; // positionable audio playback object
    
    // Buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    
    //==============================================================================
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void transportStateChanged(TransportState newState);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
