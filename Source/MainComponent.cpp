#include "MainComponent.h"

MainComponent::MainComponent() : state(Stopped)
{
    // set window size
    setSize (600, 400);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    // Configure the GUI buttons
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);
    
    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);
    
    addAndMakeVisible(&pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    pauseButton.setEnabled(false);
    
    addAndMakeVisible(&reverbSlider);
    reverbSlider.addListener(this);
    reverbSlider.setValue(0.0f);
    
    addAndMakeVisible(&reverbLabel);
    reverbLabel.setText("Reverb", juce::dontSendNotification);
    reverbLabel.attachToComponent(&reverbSlider, false);
    
    //==============================================================================
    addAndMakeVisible(&slowSlider);
    slowSlider.addListener(this);
    slowSlider.setRange(1, 10, 1);
    slowSlider.setValue(0.0f);
    
    addAndMakeVisible(&slowLabel);
    slowLabel.setText("Slow", juce::dontSendNotification);
    slowLabel.attachToComponent(&slowSlider, false);
    //==============================================================================
    
    // Configure formatManager to read wav and aiff files
    formatManager.registerBasicFormats();
    
    // listen for when the state of transport changes and call the changeListener callback function
    transport.addChangeListener(this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    
    // delete the reader pointer
    delete reader;
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    reverb.setSampleRate(sampleRate);
    //==============================================================================
    reverb.setParameters(reverbParams);
    //==============================================================================
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
//    // Right now we are not producing any data, in which case we need to clear the buffer
//    // (to prevent the output of random noise)
//    bufferToFill.clearActiveBufferRegion();
    
    transport.getNextAudioBlock(bufferToFill);
    
    // get pointer to each channel of buffer
    float* left = bufferToFill.buffer->getWritePointer(0);
    float* right = bufferToFill.buffer->getWritePointer(1);
    // apply reverb
    reverb.processStereo(left, right, bufferToFill.numSamples);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // Give the window a black background
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (CustomLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // Set position and size of the GUI buttons
    openButton.setBounds(100, 90, 150, 30);
    playButton.setBounds(100, 130, 150, 30);
    stopButton.setBounds(100, 170, 150, 30);
    pauseButton.setBounds(100, 210, 150, 30);
    reverbSlider.setBounds(300, 90, 100, 100);
    slowSlider.setBounds(450, 90, 100, 100);
}

//==============================================================================

void MainComponent::openButtonClicked()
{
    // Choose a file
    juce::FileChooser chooser("Choose a Wav file...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav;*.aiff", true, false, nullptr);
    
    // If the user chooses a file
    if (chooser.browseForFileToOpen())
    {
        playButton.setEnabled(true);
        
        // Get the chosen file
        juce::File loadedFile = chooser.getResult();
        // Read the file
        reader = formatManager.createReaderFor(loadedFile);
        
        slowAudio();
    }
}

void MainComponent::playButtonClicked()
{
    transportStateChanged(Starting);
}

void MainComponent::stopButtonClicked()
{
    transportStateChanged(Stopping);
}

void MainComponent::transportStateChanged(TransportState newState)
{
    if (newState != state)
    {
        state = newState;
        
        switch (state) {
            case Stopped:
                isPaused = false;
                playButton.setEnabled(true);
                pauseButton.setEnabled(false);
                transport.setPosition(0.0); // Set playhead at beginning of audio
                break;
            case Starting:
                isPaused = false;
                stopButton.setEnabled(true);
                playButton.setEnabled(false);
                pauseButton.setEnabled(true);
                transport.start();
                break;
            case Playing:
                isPaused = false;
                playButton.setEnabled(false);
                stopButton.setEnabled(true);
                pauseButton.setEnabled(true);
                break;
            case Stopping:
                isPaused = false;
                playButton.setEnabled(true);
                stopButton.setEnabled(false);
                transport.stop();
                transport.setPosition(0.0); // Set playhead at beginning of audio
                pauseButton.setEnabled(false);
                break;
            case Paused:
                isPaused = true;
                playButton.setEnabled(true);
                stopButton.setEnabled(true);
                transport.stop();
                pauseButton.setEnabled(false);
                break;
        }
    }
}

void MainComponent::pauseButtonClicked()
{
    transportStateChanged(Paused);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transport)
    {
        if (isPaused) {
            transportStateChanged(Paused);
        }
        else if (transport.isPlaying()) {
            transportStateChanged(Playing);
        } else {
            transportStateChanged(Stopped);
        }
    }
}

int MainComponent::getDestIndex(int sourceSampleNum, int interval)
{
    int samplesToDuplicate, destIndex;
    
    if (sourceSampleNum % interval == 0) {
        samplesToDuplicate = sourceSampleNum / interval;
    } else {
        samplesToDuplicate = (sourceSampleNum/interval) + 1;
    }
    
    destIndex = samplesToDuplicate * 2 + sourceSampleNum - samplesToDuplicate;
    
    return destIndex;
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &reverbSlider)
    {
        float val = (float)reverbSlider.getValue();
        // convert value to the range 0-1
        val /= 100;
        // set wet level to val
        reverbParams.wetLevel = val;
        // set dry level to 1-val
        reverbParams.dryLevel = 1.0f - val;
        
        reverb.setParameters(reverbParams);
    }
    
    else if (slider == &slowSlider)
    {
        std::cout << "slow = " << slowSlider.getValue() << std::endl;
    }
}

void MainComponent::slowAudio()
{
    int interval = 6;
    
    // set slowBuffer's size to hold twice as many samples as reader
    slowBuffer.setSize(2, 2 * (int) reader->lengthInSamples, false, true, false);
    slowBuffer.clear();
    
    // tempBuffer to read the audio stream into
    juce::AudioBuffer<float> tempBuffer; //todo use a pointer and then delete
    tempBuffer.setSize(2, (int) reader->lengthInSamples, false, true, false);
    tempBuffer.clear();
    
    // read audio data into tempBuffer
    reader->read(tempBuffer.getArrayOfWritePointers(), tempBuffer.getNumChannels(), 0, (int) reader->lengthInSamples);
    
    // copy each sample from tempBuffer to slowBuffer and duplicate every nth sample, where n is 10-slowValue
    for (int sourceSampleIX = 0; sourceSampleIX < tempBuffer.getNumSamples(); sourceSampleIX++)
    {
        // calculate the index to write the sample and write the sample for each channel
        int destSampleIX = getDestIndex(sourceSampleIX, interval);
        slowBuffer.setSample(0, destSampleIX, tempBuffer.getSample(0, sourceSampleIX)); // channel 0
        slowBuffer.setSample(1, destSampleIX, tempBuffer.getSample(1, sourceSampleIX)); // channel 1

        // if sourceSampleIX is a multiple of the interval: copy the samples to the next index also
        if (sourceSampleIX % interval == 0)
        {
            slowBuffer.setSample(0, destSampleIX+1, tempBuffer.getSample(0, sourceSampleIX)); // channel 0
            slowBuffer.setSample(1, destSampleIX+1, tempBuffer.getSample(1, sourceSampleIX)); // channel 1
        }
    }
    
    if (reader != nullptr) {
        // Get the file ready to play
        std::unique_ptr<juce::MemoryAudioSource> tempSource(new juce::MemoryAudioSource(slowBuffer, false));
        // set transport source to the data that tempSource is pointing to
        transport.setSource(tempSource.get());
        transportStateChanged(Stopped);
        // Pass the data to playSource and release leftover memory from tempSource
        playSource.reset(tempSource.release());
    }
    
    
    return;
}
