#pragma once

#include <JuceHeader.h>
#include <iostream>
#include "CustomLookAndFeel.h"
#include "RotarySlider.h"
#include "NameLabel.h"

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener, juce::Slider::Listener
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
    CustomLookAndFeel customLookAndFeel;
    juce::Reverb::Parameters reverbParams{0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f};
    juce::Reverb reverb;
    
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Paused,
        Stopping
    };
    
    TransportState state; // Keeps track of the state of audio playback
    juce::AudioFormatManager formatManager; // Controls what audio formats are allowed (.wav and .aiff)
    std::unique_ptr<juce::MemoryAudioSource> playSource; // plays data received from tempSource
    juce::AudioTransportSource transport; // positionable audio playback object
    std::unique_ptr<juce::AudioFormatReader> reader;
    juce::AudioBuffer<float> slowBuffer; // will hold slowed audio data
    
    // GUI controls
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton pauseButton;
    NameLabel reverbLabel;
    RotarySlider reverbSlider;
    NameLabel slowLabel;
    RotarySlider slowSlider;
    juce::TextButton lockButton;
    
    bool isPaused;
    int slowInterval;
    bool slowLocked;
    
    //==============================================================================
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void transportStateChanged(TransportState newState);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    int getDestIndex(int sourceSampleNum, int interval); // calculates destination index based on source index and interval of duplicated samples
    void sliderValueChanged(juce::Slider* slider) override;
    void lockButtonClicked();
    void slowAudio();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
