#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByteToneAudioProcessorEditor::ByteToneAudioProcessorEditor (ByteToneAudioProcessor& p)
    : AudioProcessorEditor (&p), Thread("BufferReleaseThread"), audioProcessor (p)
{
    setResizable(true, true);
    setResizeLimits(400, 200, 3000, 3000);

    juce::LookAndFeel::setDefaultLookAndFeel(&lf);

    textEditor.setFont(textEditor.getFont().withHeight(lf.getDefaultFontHeight()));
    textEditor.setMultiLine(true);
    addAndMakeVisible(textEditor);

    textEditor.setText("t * ((t>>12 | t>>9) & (t>>6) & 50)");

    runButton.setButtonText("Run");
    runButton.onClick = [this] { 
        code = textEditor.getText();
        evaluateCode();
    };
    addAndMakeVisible(runButton);

    addAndMakeVisible(sourceSampleRateMenu);

    sourceSampleRateMenu.addItem("8kHz", 1);
    sourceSampleRateMenu.addItem("11kHz", 2);
    sourceSampleRateMenu.addItem("22kHz", 3);
    sourceSampleRateMenu.addItem("32kHz", 4);
    sourceSampleRateMenu.addItem("44kHz", 5);

    sourceSampleRateMenu.onChange = [this] { sourceSampleRateChanged(); };
    sourceSampleRateMenu.setSelectedId(1);

    setSize (400, 300);

    startThread();
}

ByteToneAudioProcessorEditor::~ByteToneAudioProcessorEditor()
{
    stopThread(1000);
}

void ByteToneAudioProcessorEditor::run()
{
    while (!threadShouldExit())
    {
        checkForBuffersToFree();
        wait(500);
    }
}

void ByteToneAudioProcessorEditor::sourceSampleRateChanged()
{
    switch (sourceSampleRateMenu.getSelectedId())
    {
        case 1: sourceSampleRate = 8000; break;
        case 2: sourceSampleRate = 11000; break;
        case 3: sourceSampleRate = 22000; break;
        case 4: sourceSampleRate = 32000; break;
        case 5: sourceSampleRate = 44000; break;
        default: break;
    }

    evaluateCode();
}

void ByteToneAudioProcessorEditor::checkForBuffersToFree()
{
    for (auto i = buffers.size(); --i >= 0;)
    {
        ReferenceCountedBuffer::Ptr buffer(buffers.getUnchecked(i));

        if (buffer->getReferenceCount() == 2)
            buffers.remove(i);
    }
}

void ByteToneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    const juce::String text = juce::String(ProjectInfo::projectName) + " v" + juce::String(ProjectInfo::versionString);
    g.drawFittedText (text, getLocalBounds().removeFromTop(20).withLeft(6), juce::Justification::left, 1);
}

void ByteToneAudioProcessorEditor::resized()
{
    const int headerHeight = 20;
    const int buttonWidth = 40;
    const int comboWidth = 80;

    auto r = getLocalBounds();
    auto top = r.removeFromTop(headerHeight);
    runButton.setBounds(top.removeFromRight(buttonWidth));
    sourceSampleRateMenu.setBounds(top.removeFromRight(comboWidth));
    textEditor.setBounds(getLocalBounds().withTop(20));
}

void ByteToneAudioProcessorEditor::evaluateCode()
{
    if (code.trim().isEmpty()) {
        return;
    }

    const int lengthInSamples = sourceSampleRate * 30;
    const juce::AudioSampleBuffer tempBuffer = generateFromText(code, lengthInSamples);
    ReferenceCountedBuffer::Ptr newBuffer = resampleBuffer(code, tempBuffer, sourceSampleRate);
    audioProcessor.setCurrentBuffer(newBuffer);
    buffers.add(newBuffer);
}

juce::AudioSampleBuffer ByteToneAudioProcessorEditor::generateFromText(juce::String text, int lengthInSamples)
{
    const juce::Array<int> result = interpreter.generate(text, lengthInSamples);

    int destSample = 0;
    juce::AudioSampleBuffer tempBuffer(2, lengthInSamples);

    for (const int integer : result)
    {
        tempBuffer.setSample(0, destSample, integerToSample(integer));
        tempBuffer.setSample(1, destSample, integerToSample(integer));
        destSample++;
    }

    return tempBuffer;
}

ReferenceCountedBuffer::Ptr ByteToneAudioProcessorEditor::resampleBuffer(const juce::String name, const juce::AudioSampleBuffer& buffer, int sourceSampleRate)
{
    double ratio = sourceSampleRate / processor.getSampleRate();

    ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(
        name, processor.getNumOutputChannels(), buffer.getNumSamples() / ratio);

    float** outputs = newBuffer->getAudioSampleBuffer()->getArrayOfWritePointers();
    const float** inputs = buffer.getArrayOfReadPointers();

    for (int c = 0; c < newBuffer->getAudioSampleBuffer()->getNumChannels(); c++)
    {
        juce::LagrangeInterpolator resampler;
        resampler.process(ratio, inputs[c], outputs[c], newBuffer->getAudioSampleBuffer()->getNumSamples());
    }

    return newBuffer;
}

float ByteToneAudioProcessorEditor::integerToSample(int integer)
{
    const int max = 255;
    return juce::jmap((float)(integer & max), 0.0f, (float)max, -1.0f, 1.0f);
}
