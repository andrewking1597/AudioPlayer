#include "MainComponent.h"

MainComponent::MainComponent() : state(NoFile), queueDisplay("Queue", &queueModel), prevSelectedRow(-1)
{
    this->addKeyListener(this);
    
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
    
    //==============================================================================
    // Configure the GUI buttons and sliders
    addAndMakeVisible(&titleLabel);
    titleLabel.setText("Slow + Reverb", juce::dontSendNotification);
    titleLabel.setFont(50.f);
    
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, newGreen);
    playButton.setEnabled(false);
    
    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, newRed);
    stopButton.setEnabled(false);
    
    addAndMakeVisible(&pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour(juce::TextButton::buttonColourId, newBlue);
    pauseButton.setEnabled(false);
    
    addAndMakeVisible(&reverbLabel);
    reverbLabel.setText("Reverb", juce::dontSendNotification);
    reverbLabel.attachToComponent(&reverbSlider, false);
    
    addAndMakeVisible(&reverbSlider);
    reverbSlider.addListener(this);
    reverbSlider.setValue(0.0f);
    reverbSlider.setPathColor(newGreen);
    
    addAndMakeVisible(&slowLabel);
    slowLabel.setText("Slow", juce::dontSendNotification);
    slowLabel.attachToComponent(&slowSlider, false);
    
    addAndMakeVisible(&slowSlider);
    slowSlider.addListener(this);
    slowSlider.setValue(0.0f);
    slowSlider.setPathColor(newPink);
    //==============================================================================
    
    addAndMakeVisible(&queueDisplay);
    queueDisplay.setWantsKeyboardFocus(false);
    
    // Configure formatManager to read wav and aiff files
    formatManager.registerBasicFormats();
    // listen for when the state of transport changes and call the changeListener callback function
    transport.addChangeListener(this);
    // call transportStateChanged to set up initial state
    transportStateChanged(NoFile);
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
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (CustomLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    titleLabel.setBounds(0, 0, getWidth(), 120);
    
    openButton.setBounds(40, 140, 150, 30); // (75, 150) w = 150, h = 30
    queueDisplay.setBounds(40, 185, 150, 100);
    
    reverbSlider.setBounds(223, 163 + reverbLabel.getHeight()*0.5, 100, 100);
    slowSlider.setBounds(356, 163 + slowLabel.getHeight()*0.5, 100, 100);
    
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
        // Get the chosen file and add to queue
        juce::File loadedFile = chooser.getResult();
        queueModel.addItem(loadedFile);
        queueDisplay.updateContent();
        
        // if this is the only file in the queue, set reader
        if (queueModel.getNumRows() == 1)
        {
            reader.reset(formatManager.createReaderFor(loadedFile));
            // allocate space in originalBuffer
            originalBuffer.setSize(2, (int) reader->lengthInSamples, false, true, false);
            reader->read(originalBuffer.getArrayOfWritePointers(), originalBuffer.getNumChannels(), 0, (int) reader->lengthInSamples);
            prepareAudio();
            transportStateChanged(Stopped);
        }
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
    TransportState oldState = state;
    state = newState;
    
    switch (state) {
        case NoFile:
            isPaused = false;
            playButton.setEnabled(false);
            stopButton.setEnabled(false);
            pauseButton.setEnabled(false);
            reverbSlider.setValue(0.0);
            slowSlider.setValue(0.0);
            transport.setPosition(0.0);
            break;
        case Done:
            stopTimer();
            isPaused = false;
            transport.stop();
            queueModel.popHead();
            queueDisplay.updateContent();
            if (queueModel.getNumRows() > 0) {
                reader.reset(formatManager.createReaderFor(queueModel.getHead()));
                originalBuffer.setSize(2, (int) reader->lengthInSamples, false, true, false);
                reader->read(originalBuffer.getArrayOfWritePointers(), originalBuffer.getNumChannels(), 0, (int) reader->lengthInSamples);
                transport.setPosition(0.0);
                prepareAudio();
//                transportStateChanged(Playing);
                transportStateChanged(oldState);
            } else {
                transportStateChanged(NoFile);
            }
            break;
        case Stopped:
            stopTimer();
            isPaused = false;
            transport.stop();
            transport.setPosition(0.0);
            stopButton.setEnabled(false);
            pauseButton.setEnabled(false);
            playButton.setEnabled(true);
            break;
        case Playing:
            isPaused = false;
            playButton.setEnabled(false);
            stopButton.setEnabled(true);
            pauseButton.setEnabled(true);
            transport.start();
            startTimer(10);
            break;
        case Paused:
            stopTimer();
            isPaused = true;
            transport.stop();
            pauseButton.setEnabled(false);
            playButton.setEnabled(true);
            stopButton.setEnabled(true);
            break;
    }
}

void MainComponent::pauseButtonClicked()
{
    transportStateChanged(Paused);
}

void MainComponent::prepareAudio()
{
    int newInterval = 100 / slowSlider.getValue();
    
    // if slider set to 0: set interval to be greater than numSamples so audio won't be slowed at all
    if (slowSlider.getValue() == 0)
    {
        newInterval = (float)originalBuffer.getNumSamples()+1;
    }
    
    slowInterval = newInterval;
    slowAudio(slowInterval);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
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
    if (slider == &reverbSlider) {
        reverbSliderValueChanged();
    } else if (slider == &slowSlider) {
        slowSliderValueChanged();
    }
    
    return;
}

void MainComponent::reverbSliderValueChanged()
{
    // convert reverb value to the range 0-1
    float val = (float)reverbSlider.getValue() / 100;
    
    // Use val to set wet and dry levels
    reverbParams.wetLevel = val;
    reverbParams.dryLevel = 1.0f - val;
    
    reverb.setParameters(reverbParams);
    
    return;
}

void MainComponent::slowSliderValueChanged()
{
    // Keep track of whether audio was playing or not
    // (we'll resume playback if it was playing)
    TransportState oldState = state;
    
    // Temporarily pause audio playback
    transportStateChanged(Paused);
    
    double currPosition = transport.getCurrentPosition(); // get playhead position (in seconds)
    double positionRatio = currPosition / transport.getLengthInSeconds();
    
    // if slider set to 0: set interval to be greater than numSamples so audio won't be slowed at all
    int newInterval;
    if (slowSlider.getValue() > 0) {
        newInterval = 100 / slowSlider.getValue();
    } else {
        newInterval = originalBuffer.getNumSamples() + 1;
    }
    
    // Slow the audio
    slowInterval = newInterval;
    slowAudio(slowInterval);
    
    // Adjust playhead position to account for the duplicated samples
    double newPosition = positionRatio * transport.getLengthInSeconds();
    transport.setPosition(newPosition);
    
    // Revert to the previous state
    transportStateChanged(oldState);
    // If paused, adjust playhead to account for the duplicated samples
    if (state == Paused) {
        transport.setPosition(newPosition);
    }
}

void MainComponent::slowAudio(int interval)
{
    // set slowBuffer's size to hold enough samples for the slowed audio
    slowBuffer.setSize(2, 1 + (int)reader->lengthInSamples + (int)reader->lengthInSamples / interval, false, true, false);
    slowBuffer.clear(); //todo unnecessary bc clearExtraSpace is set to true in the setSize call =============A
    
    // copy each sample from tempBuffer to slowBuffer and duplicate every interval-th sample
    for (int sourceSampleIX = 0; sourceSampleIX < originalBuffer.getNumSamples(); sourceSampleIX++)
    {
        // calculate the index to write the sample and write the sample for each channel
        int destSampleIX = getDestIndex(sourceSampleIX, interval);
        slowBuffer.setSample(0, destSampleIX, originalBuffer.getSample(0, sourceSampleIX)); // channel 0
        slowBuffer.setSample(1, destSampleIX, originalBuffer.getSample(1, sourceSampleIX)); // channel 1

        // if sourceSampleIX is a multiple of the interval: copy the samples to the next index also
        if (sourceSampleIX % interval == 0)
        {
            slowBuffer.setSample(0, destSampleIX+1, originalBuffer.getSample(0, sourceSampleIX)); // channel 0
            slowBuffer.setSample(1, destSampleIX+1, originalBuffer.getSample(1, sourceSampleIX)); // channel 1
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

void MainComponent::timerCallback()
{
    // if stream is finished: change transportState
    if (transport.getCurrentPosition() >= transport.getLengthInSeconds() && transport.getCurrentPosition() > 0)
    {
        transportStateChanged(Done);
    }
    
    // if reverbSlider is set to 0, make sure it registers as a change
    // see issue #26 for details: https://github.com/andrewking1597/SlowReverbPlayer/issues/26
    if (reverbSlider.getValue() == 0 && reverbParams.wetLevel != 0) {
        reverbSliderValueChanged();
    }
    
    // compare prevSelectedRow with the currently selected row
    // temporary fix for issue #24: https://github.com/andrewking1597/SlowReverbPlayer/issues/24
    if (prevSelectedRow != queueDisplay.getSelectedRow())
    {
        // update prevSelectedRow
        prevSelectedRow = queueDisplay.getSelectedRow();
        //? hopefully this readies the delete key?
        transportStateChanged(state);
    }
}

bool MainComponent::keyPressed(const juce::KeyPress &key, juce::Component* originatingComponent)
{
    // note: delete key has keycode 127, x has keycode 88
    //todo! only works if the state has been changed since the song has been selected.
    //todo! ex: if you select row 1 and click del nothing will happen; but if you select
    //todo!     row 1 then click play (or stop or pause), it deletes the song as expected.
    if (key.isKeyCode(88) || key.isKeyCode(127) || key.isKeyCode(8))
    {
        DBG("key pressed!");
        // get index of selected file (queueDisplay)
        int selectedRow = queueDisplay.getSelectedRow();
        std::cout << selectedRow << std::endl;

        if (selectedRow == 0) {
            transportStateChanged(Done);
        } else if (selectedRow > 0) {
            // delete that index from queueModel
            queueModel.deleteRow(selectedRow);
            // update queueDisplay
            queueDisplay.updateContent();
        }
    }
    
    return true;
}
