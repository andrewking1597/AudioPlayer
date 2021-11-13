#include "MainComponent.h"

MainComponent::MainComponent() : state(NoFile), queueDisplay("Queue", &queueModel)
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
    addAndMakeVisible(&titleLabel);
    titleLabel.setText("Slow + Reverb", juce::dontSendNotification);
    titleLabel.setFont(50.f);
    
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, offWhite);
    playButton.setEnabled(false);
    
    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, offWhite);
    stopButton.setEnabled(false);
    
    addAndMakeVisible(&pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour(juce::TextButton::buttonColourId, offWhite);
    pauseButton.setEnabled(false);
    
    addAndMakeVisible(&reverbLabel);
    reverbLabel.setText("Reverb", juce::dontSendNotification);
    reverbLabel.attachToComponent(&reverbSlider, false);
    
    addAndMakeVisible(&reverbSlider);
    reverbSlider.addListener(this);
    reverbSlider.setValue(0.0f);
    reverbSlider.setPathColor(abkGreen);
    
    addAndMakeVisible(&slowLabel);
    slowLabel.setText("Slow", juce::dontSendNotification);
    slowLabel.attachToComponent(&slowSlider, false);
    
    addAndMakeVisible(&slowSlider);
    slowSlider.addListener(this);
    slowSlider.setValue(0.0f);
    slowSlider.setPathColor(abkPink);
    
    addAndMakeVisible(&setButton);
    setButton.setButtonText("Set");
    setButton.onClick = [this] { setButtonClicked(); };
    setButton.setColour(juce::TextButton::buttonColourId, abkPink);
    setButton.setEnabled(false);
    
    // Configure formatManager to read wav and aiff files
    formatManager.registerBasicFormats();
    
    // listen for when the state of transport changes and call the changeListener callback function
    transport.addChangeListener(this);
    
    // call transportStateChanged to set up initial state
    transportStateChanged(NoFile);
    
    //==============================================================================
    queueModel.addItem("TestOne");
    queueModel.addItem("TestTwo");
    queueDisplay.updateContent();
    addAndMakeVisible(&queueDisplay);
    //==============================================================================
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    reverb.setSampleRate(sampleRate);
    reverb.setParameters(reverbParams);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    transport.getNextAudioBlock(bufferToFill);
    
    // get pointer to each channel of buffer
    float* left = bufferToFill.buffer->getWritePointer(0);
    float* right = bufferToFill.buffer->getWritePointer(1);
    // apply reverb
    reverb.processStereo(left, right, bufferToFill.numSamples);
    
    //==============================================================================
    // if stream is finished: set transportState to Stopped
    if (transport.getCurrentPosition() >= transport.getLengthInSeconds() && transport.getCurrentPosition() > 0)
    {
        const juce::MessageManagerLock mmLock; // acquire access to the message thread before calling component methods
                                               // note: will automatically free the lock when it goes out of scope
        if (mmLock.lockWasGained())
        {
            transportStateChanged(Stopped);
        }
    }
    //==============================================================================
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (CustomLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    titleLabel.setBounds(0, 0, getWidth(), 120);
    
    openButton.setBounds(40, 140, 150, 30); // (75, 150) w = 150, h = 30
    queueDisplay.setBounds(40, 185, 150, 100);
    
    reverbSlider.setBounds(223, 163 + reverbLabel.getHeight()*0.5, 100, 100);
    slowSlider.setBounds(356, 163 + slowLabel.getHeight()*0.5, 100, 100);
    setButton.setBounds(366, 284 + slowLabel.getHeight()*0.5, 80, 25);
    
    playButton.setBounds(489, 153, 70, 30); // (75, 190) w = 70, h = 30
    pauseButton.setBounds(489, 198, 70, 30); // (155, 190) w = 70, h = 30
    stopButton.setBounds(489, 243, 70, 30); // (75, 230) w = 150, h = 30
}

//==============================================================================

void MainComponent::openButtonClicked()
{
    // Choose a file
    juce::FileChooser chooser("Choose a Wav file...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav;*.aiff", true, false, nullptr);
    
    // If the user chooses a file
    if (chooser.browseForFileToOpen())
    {
        // Get the chosen file
        juce::File loadedFile = chooser.getResult();
        // Read the file
        reader.reset(formatManager.createReaderFor(loadedFile));
        
        transportStateChanged(Stopped);
    }
}

void MainComponent::playButtonClicked()
{
    transportStateChanged(Playing);
}

void MainComponent::stopButtonClicked()
{
    transportStateChanged(Stopped);
}

void MainComponent::transportStateChanged(TransportState newState)
{
    state = newState;
    
    switch (state) {
        case NoFile:
            isPaused = false;
            playButton.setEnabled(false);
            stopButton.setEnabled(false);
            pauseButton.setEnabled(false);
            setButton.setEnabled(false);
            break;
        case Stopped:
            isPaused = false;
            transport.stop();
            transport.setPosition(0.0);
            stopButton.setEnabled(false);
            pauseButton.setEnabled(false);
            playButton.setEnabled(true);
            setButton.setEnabled(true);
            break;
        case Playing:
            isPaused = false;
            playButton.setEnabled(false);
            setButton.setEnabled(false);
            stopButton.setEnabled(true);
            pauseButton.setEnabled(true);
            transport.start();
            break;
        case Paused:
            isPaused = true;
            transport.stop();
            pauseButton.setEnabled(false);
            playButton.setEnabled(true);
            stopButton.setEnabled(true);
            setButton.setEnabled(true);
            break;
    }
}

void MainComponent::pauseButtonClicked()
{
    transportStateChanged(Paused);
}

void MainComponent::setButtonClicked()
{
    int newInterval = reader->lengthInSamples / (reader->lengthInSamples * slowSlider.getValue() / 100);
    
    // if slider set to 0: set interval to be greater than numSamples so audio won't be slowed at all
    if (slowSlider.getValue() == 0)
    {
        newInterval = (float)reader->lengthInSamples + 1;
    }
    
    if (newInterval != slowInterval)
    {
        slowInterval = newInterval;
        slowAudio(slowInterval);
    }
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
            isPaused = false; // just in case
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
}

void MainComponent::slowAudio(int interval)
{
    // set slowBuffer's size to hold enough samples for the slowed audio
    slowBuffer.setSize(2, 1 + (int)reader->lengthInSamples + (int)reader->lengthInSamples / interval, false, true, false);
    slowBuffer.clear(); //todo unnecessary bc clearExtraSpace is set to true in the setSize call =============A
    
    // tempBuffer to read the audio stream into
    juce::AudioBuffer<float> tempBuffer; //todo use a pointer and then delete
    tempBuffer.setSize(2, (int) reader->lengthInSamples, false, true, false);
    tempBuffer.clear();
    
    // read audio data into tempBuffer
    reader->read(tempBuffer.getArrayOfWritePointers(), tempBuffer.getNumChannels(), 0, (int) reader->lengthInSamples);
    
    // copy each sample from tempBuffer to slowBuffer and duplicate every interval-th sample
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
        // Pass the data to playSource
        std::unique_ptr<juce::MemoryAudioSource> tempSource(new juce::MemoryAudioSource(slowBuffer, false));
        transport.setSource(tempSource.get()); // set transport source to the data that tempSource is pointing to
        transportStateChanged(Stopped);
        playSource.reset(tempSource.release());
    }
    
    
    return;
}


