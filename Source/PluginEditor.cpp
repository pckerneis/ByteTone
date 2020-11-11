#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByteToneAudioProcessorEditor::ByteToneAudioProcessorEditor (ByteToneAudioProcessor& p)
    : AudioProcessorEditor (&p), Thread("BufferReleaseThread"), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&lf);

    textEditor.setFont(textEditor.getFont().withHeight(16.0f));
    textEditor.setMultiLine(true);
    addAndMakeVisible(textEditor);

    textEditor.setText("t * ((t>>12 | t>>9) & (t>>6) & 50)");

    runButton.setButtonText("Run");
    runButton.onClick = [this] { evaluateText(); };
    addAndMakeVisible(runButton);

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

    auto r = getLocalBounds();
    auto top = r.removeFromTop(headerHeight);
    runButton.setBounds(top.removeFromRight(buttonWidth));
    textEditor.setBounds(getLocalBounds().withTop(20));
}

void ByteToneAudioProcessorEditor::evaluateText()
{
    const int lengthInSamples = 8000 * 30;
    const auto text = textEditor.getText();
    const juce::AudioSampleBuffer tempBuffer = generateFromText(text, lengthInSamples);
    ReferenceCountedBuffer::Ptr newBuffer = resampleBuffer(text, tempBuffer, 8000);
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
    double ratio = 8000 / processor.getSampleRate();

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
