#pragma once

#include <JuceHeader.h>
#include <iostream>
#include "CustomLookAndFeel.h"
#include "RotarySlider.h"
#include "NameLabel.h"
#include "QueueModel.h"

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener, juce::Slider::Listener, public juce::Timer
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
    
    void timerCallback() override;

private:
    CustomLookAndFeel customLookAndFeel;
    juce::Reverb::Parameters reverbParams{0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f};
    juce::Reverb reverb;
    bool isPaused;
    int slowInterval;
    
    enum TransportState
    {
        NoFile,
        Done,
        Stopped,
        Playing,
        Paused
    };
    
    TransportState state; // Keeps track of the state of audio playback
    juce::AudioFormatManager formatManager; // Controls what audio formats are allowed (.wav and .aiff)
    std::unique_ptr<juce::MemoryAudioSource> playSource; // plays data received from tempSource
    juce::AudioTransportSource transport; // positionable audio playback object
    std::unique_ptr<juce::AudioFormatReader> reader;
    juce::AudioBuffer<float> slowBuffer; // will hold slowed audio data
    
    QueueModel queueModel;
    juce::ListBox queueDisplay;
    
    // GUI controls
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton pauseButton;
    NameLabel reverbLabel;
    RotarySlider reverbSlider;
    NameLabel slowLabel;
    RotarySlider slowSlider;
    juce::TextButton setButton;
    NameLabel titleLabel;
    
    //==============================================================================
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void transportStateChanged(TransportState newState);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    int getDestIndex(int sourceSampleNum, int interval); // calculates destination index based on source index and interval of duplicated samples
    void sliderValueChanged(juce::Slider* slider) override;
    void setButtonClicked();
    void slowAudio(int interval=0);
    
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour offWhite2 = juce::Colour::fromRGB(229, 229, 229);
    juce::Colour newGreen = juce::Colour::fromRGB(62, 218, 121);
    juce::Colour newRed = juce::Colour::fromRGB(249, 62, 59);
    juce::Colour newBlue = juce::Colour::fromRGB(13, 81, 230);
    juce::Colour newPink = juce::Colour::fromRGB(239, 59, 243);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
