#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByteToneAudioProcessorEditor::ByteToneAudioProcessorEditor (ByteToneAudioProcessor& p)
    : AudioProcessorEditor (&p), Thread("BufferReleaseThread"), audioProcessor (p), sourceSampleRate(8000), evaluationMode(BYTE)
{
    setResizable(true, true);
    setResizeLimits(400, 200, 3000, 3000);

    juce::LookAndFeel::setDefaultLookAndFeel(&lf);

    console.setFont(textEditor.getFont().withHeight(lf.getDefaultFontHeight()));
    console.setMultiLine(true);
    console.setReadOnly(true);
    console.setColour(TextEditor::textColourId, juce::Colours::red);
    addAndMakeVisible(console);

    textEditor.setFont(textEditor.getFont().withHeight(lf.getDefaultFontHeight()));
    textEditor.setMultiLine(true, true);
    textEditor.setReturnKeyStartsNewLine(true);
    addAndMakeVisible(textEditor);

    textEditor.setText("t * ((t>>12 | t>>9) & (t>>6) & 50)");

    runButton.setButtonText("Run");
    runButton.onClick = [this] { 
        code = textEditor.getText();
        evaluateCode();
    };
    addAndMakeVisible(runButton);

    addAndMakeVisible(sampleRateSlider);

    sampleRateSlider.setSliderStyle(Slider::LinearBar);
    sampleRateSlider.setTextValueSuffix("Hz");
    sampleRateSlider.setRange(juce::Range<double>(1000.0, 88000.0), 1);
    sampleRateSlider.setValue(8000);

    sampleRateSlider.onDragEnd = [this] { sourceSampleRateChanged(); };
    // TODO : filter change events
    // sampleRateSlider.onValueChange = [this] { sourceSampleRateChanged(); };

    addAndMakeVisible(evaluationModeMenu);

    evaluationModeMenu.addItem("byte", 1);
    evaluationModeMenu.addItem("float", 2);

    evaluationModeMenu.onChange = [this] { evaluateModeChanged(); };
    evaluationModeMenu.setSelectedId(1);

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
    sourceSampleRate = sampleRateSlider.getValue();
    evaluateCode();
}

void ByteToneAudioProcessorEditor::evaluateModeChanged()
{
    switch (evaluationModeMenu.getSelectedId())
    {
    case 1: evaluationMode = EvaluationMode::BYTE; break;
    case 2: evaluationMode = EvaluationMode::FLOAT; break;
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

    const int consoleHeight = 60;
    g.setColour(juce::Colours::darkgrey);
    g.drawRect(getLocalBounds().removeFromBottom(consoleHeight).removeFromTop(1));
}

void ByteToneAudioProcessorEditor::resized()
{
    const int consoleHeight = 60;
    const int headerHeight = 28;
    const int buttonWidth = 40;
    const int comboWidth = 80;

    auto r = getLocalBounds();
    auto top = r.removeFromTop(headerHeight);
    runButton.setBounds(top.removeFromRight(buttonWidth));
    evaluationModeMenu.setBounds(top.removeFromRight(comboWidth));
    sampleRateSlider.setBounds(top.removeFromRight(comboWidth));
    console.setBounds(r.removeFromBottom(consoleHeight));
    textEditor.setBounds(getLocalBounds().withTop(20));
}

void ByteToneAudioProcessorEditor::evaluateCode()
{
    if (code.trim().isEmpty()) {
        return;
    }

    console.setText("");

    try
    {
        const int lengthInSamples = sourceSampleRate * 30;
        const juce::AudioSampleBuffer tempBuffer = generateFromText(code, lengthInSamples);
        ReferenceCountedBuffer::Ptr newBuffer = resampleBuffer(code, tempBuffer, sourceSampleRate);
        audioProcessor.setCurrentBuffer(newBuffer);
        buffers.add(newBuffer);
    }
    catch (ParseError& exception)
    {
        console.setText(exception.getMessage());
    }
    catch (ScanError& exception)
    {
        console.setText(exception.getMessage());
    }
}

juce::AudioSampleBuffer ByteToneAudioProcessorEditor::generateFromText(juce::String text, int lengthInSamples)
{
    const juce::Array<Var> result = interpreter.generate(text, lengthInSamples);

    int destSample = 0;
    juce::AudioSampleBuffer tempBuffer(2, lengthInSamples);
    bool floatMode = evaluationMode == EvaluationMode::FLOAT;

    for (const Var r : result)
    {
        float sample = floatMode ? (float)r : integerToSample((int)r);
        tempBuffer.setSample(0, destSample, sample);
        tempBuffer.setSample(1, destSample, sample);
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
