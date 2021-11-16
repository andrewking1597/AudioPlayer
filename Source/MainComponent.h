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
    
    juce::Colour blue = juce::Colour::fromFloatRGBA(0.43f, 0.83f, 1.0f, 1.0f);
    juce::Colour grey = juce::Colour::fromFloatRGBA(0.42f, 0.42f, 0.42f, 1.0f);
    juce::Colour blackGrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
    juce::Colour offWhite = juce::Colour::fromFloatRGBA(0.83f, 0.84f, 0.9f, 1.0f);
    juce::Colour mint = juce::Colour::fromFloatRGBA(0.54f, 1.0f, 0.76f, 1.0f);
    juce::Colour fireRed = juce::Colour::fromFloatRGBA(0.93f, 0.38f, 0.33f, 1.0f);
    juce::Colour abkBlue = juce::Colour::fromRGB(5, 17, 242);
    juce::Colour abkRed = juce::Colour::fromRGB(242, 5, 5);
    juce::Colour abkGreen = juce::Colour::fromRGB(90, 157, 59);
    juce::Colour abkYellow = juce::Colour::fromRGB(233, 215, 88);
    juce::Colour abkPeach = juce::Colour::fromRGB(247, 178, 183);
    juce::Colour abkPink = juce::Colour::fromRGB(212, 17, 208);
    juce::Colour abkPurple = juce::Colour::fromRGB(61, 20, 245);
    
    juce::Colour offWhite2 = juce::Colour::fromRGB(229, 229, 229);
    juce::Colour abkLightGrey = juce::Colour::fromRGB(204, 204, 204);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
