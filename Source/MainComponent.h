/**
 @file MainComponent.h
 */

#pragma once

#include <JuceHeader.h>
#include <iostream>
#include <cstring>
#include <vector>
//#include <aubio/aubio.h>
#include <Headers/aubio.h>
#include "CustomLookAndFeel.h"
#include "RotarySlider.h"
#include "NameLabel.h"
#include "QueueModel.h"

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener, juce::Slider::Listener, public juce::Timer, public juce::KeyListener
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
    
    bool keyPressed(const juce::KeyPress &key, juce::Component* originatingComponent) override;

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
    juce::AudioBuffer<float> originalBuffer; // will hold audio as it is read from file
    
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
    
    bool useTargetBpm;
    float targetBpm; //todo make a method getTargetBpm() and use that instead so it's always up to date
    juce::ToggleButton bpmButton;
    juce::TextEditor bpmInput;
    void bpmButtonClicked();
    float getFileBpm(juce::File* f);
    int getBpmInterval(float sourceBpm, float targetBpm);
    void updateSlowSliderViaBpm();
    
    //DBG
    int aubioTest();
    
    //==============================================================================
    /**
     *@brief Called when openButton is clicked.
     *Opens a fileChooser window and allows user to select a file to load into the queue. If the queue was previously
     *empty, the audio data from the selected file is read into originalBuffer and readied for playback.
     */
    void openButtonClicked();
    /**
     *@brief Called when playButton is clicked.
     *Sets state to Playing, which starts audio playback.
     */
    void playButtonClicked();
    /**
     *@brief Called when stopButton is clicked.
     *Sets state to Stopped, which stops audio playback and resets playhead to the beginning.
     */
    void stopButtonClicked();
    /**
     *@brief Called when pauseButton is clicked.
     *Sets state to Paused, which stops audio playback but keeps the playhead in its current position.
     */
    void pauseButtonClicked();
    /**
     *@brief Changes the value of state to a TransportState value.
     *@param newState  the TransportState value to set state to
     */
    void transportStateChanged(TransportState newState);
    
    /**
     *@brief Calculates the destination index based on the source index and interval of duplicated samples.
     *@param sourceSampleNum  the index of the sample in the source buffer
     *@param interval   the interval between samples to be duplicated. If every 5th sample will be duplicated, interval should be set to 5.
     *@return  the index of the destination buffer the sample should be copied to
     */
    int getDestIndex(int sourceSampleNum, int interval);
    /**
     *@brief Called when a slider is moved.
     *Calls a more specific method based on which slider was moved.
     *@param slider  the slider that was moved
     *@see reverbSliderValueChanged()
     *@see slowSliderValueChanged()
     */
    void sliderValueChanged(juce::Slider* slider) override;
    /**
     *@brief Updates the reverb parameters to reflect the new slider value
     */
    void reverbSliderValueChanged();
    /**
     *@brief Prepares the audio to be slowed and calls slowAudio()
     *@see slowAudio()
     */
    void slowSliderValueChanged();
    /**
     *@brief Fills slowedBuffer with a slowed version of the audio data in originalBuffer
     *@param interval  the interval between samples to be duplicated. If every 5th sample will be duplicated, interval should be set to 5.
     */
    void slowAudio(int interval=0);
    /**
     *@brief Prepares the audio to be slowed, then calls slowAudio()
     *Calculates the interval based on the value of slowSlider, then slows the audio
     *@see slowAudio()
     */
    void prepareAudio();
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
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
