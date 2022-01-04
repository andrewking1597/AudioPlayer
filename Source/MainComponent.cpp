#include "MainComponent.h"

MainComponent::MainComponent() : state(NoFile), queueDisplay("Queue", &queueModel), bpmInput("bpmInput")
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
    
    addAndMakeVisible(&bpmButton);
    bpmButton.setButtonText("Use BPM");
    bpmButton.changeWidthToFitText();
    bpmButton.onClick = [this] { bpmButtonClicked(); };
    
    addAndMakeVisible(&bpmInput);
    bpmInput.setInputFilter(new BpmInputFilter, true);
    
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
    
    bpmButton.setBounds(40, 300, 80, 30);
    bpmInput.setBounds(40+bpmButton.getWidth()+10, 300, 50, 30);
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
    if (bpmButton.getToggleState()) {
        updateSlowSliderViaBpm();
        return;
    }
    
    int newInterval = 100 / slowSlider.getValue();
    
    // if slider set to 0: set interval to be greater than numSamples so audio won't be slowed at all
    if (slowSlider.getValue() == 0) {
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
    // if no file is loaded, set slider back to 0
    if (state == NoFile) {
        slowSlider.setValue(0);
        return;
    }
    
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
    
}

bool MainComponent::keyPressed(const juce::KeyPress &key, juce::Component* originatingComponent)
{
    // note: delete key has keycode 127, x has keycode 88
    if (key.isKeyCode(88) || key.isKeyCode(127) || key.isKeyCode(8))
    {
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

void MainComponent::bpmButtonClicked()
{
    if (bpmButton.getToggleState()) {
        updateSlowSliderViaBpm();
    }
    return;
}

void MainComponent::updateSlowSliderViaBpm()
{
    // detect bpm of current file
    float sourceBpm = getFileBpm(queueModel.getHeadPtr());
    
    // calculate the value slowSlider should be set to to reach the target bpm
    float bpmSlowVal = 100 * (sourceBpm - getTargetBpm()) / sourceBpm;
    
    // update slowSlider
    slowSlider.setValue(bpmSlowVal);
    
    return;
}

float MainComponent::getFileBpm(juce::File* f)
{
    uint_t xsampleRate = 0; // we'll set this later
    uint_t xwindowSize = 1024;
    uint_t xhopSize = xwindowSize / 4;
    uint_t xnumFrames = 0;
    uint_t xread = 0;
    
    std::vector<float> bpmGuesses;
    std::vector<float> bpmConfidences;
    
    juce::String pathname = f->getFullPathName(); //conversion helper
    std::string stdpathname = pathname.toStdString(); //conversion helper
    const char_t* xsourcePath = stdpathname.c_str();
    aubio_source_t* xsource = new_aubio_source(xsourcePath, xsampleRate, xhopSize);
    
    if (xsampleRate == 0) xsampleRate = aubio_source_get_samplerate(xsource);
    
    // create some vectors
    fvec_t* xin = new_fvec(xhopSize); // input audio buffer
    fvec_t* xout = new_fvec(1); // output position
    
    // create tempo object
    aubio_tempo_t* xtempoObj = new_aubio_tempo("default", xwindowSize, xhopSize, xsampleRate);
    
    do {
        // put some fresh data in input vector
        aubio_source_do(xsource, xin, &xread);
        // execute tempo
        aubio_tempo_do(xtempoObj, xin, xout);
        // append to bpmGuesses and bpmConfidences
        bpmGuesses.push_back(aubio_tempo_get_bpm(xtempoObj));
        bpmConfidences.push_back(aubio_tempo_get_confidence(xtempoObj));
        xnumFrames += xread;
    } while (xread == xhopSize);
    
    // find the index of the highest confidence
    float maxConf = -1;
    float maxConf_ix = 0;
    for (int i = 0; i < bpmConfidences.size(); i++) {
        if (bpmConfidences.at(i) > maxConf) {
            maxConf = bpmConfidences.at(i);
            maxConf_ix = i;
        }
    }
    // get bpm at maxConf_ix of bpmGuesses
    float bestGuess = bpmGuesses.at(maxConf_ix);
    
    // clean up memory
    del_aubio_tempo(xtempoObj);
    del_fvec(xin);
    del_fvec(xout);
    del_aubio_source(xsource);
    aubio_cleanup();
    
    return bestGuess;
}

float MainComponent::getTargetBpm()
{
    juce::String bpmString = bpmInput.getText();
    return bpmString.getFloatValue();
}
