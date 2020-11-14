#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByteToneAudioProcessorEditor::ByteToneAudioProcessorEditor (ByteToneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p)
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

    textEditor.setText(p.getCurrentCode());

    runButton.setButtonText("Run");
    runButton.onClick = [this, &p] { 
        p.setCurrentCode(textEditor.getText());
        evaluateCode();
    };
    addAndMakeVisible(runButton);

    sampleRateSlider.setSliderStyle(Slider::LinearBar);
    sampleRateSlider.setTextValueSuffix("Hz");
    sampleRateAttachment.reset (new SliderAttachment (p.getParameters(), "sampleRate", sampleRateSlider));
    sampleRateSlider.onDragEnd = [this] { sourceSampleRateChanged(); };
    addAndMakeVisible(sampleRateSlider);
    // TODO : filter change events
    // sampleRateSlider.onValueChange = [this] { sourceSampleRateChanged(); };

    evaluationModeMenu.addItem("byte", 1);
    evaluationModeMenu.addItem("float", 2);
    evaluationModeMenu.onChange = [this] { evaluateModeChanged(evaluationModeMenu.getSelectedId() - 1); };
    evaluationModeMenu.setSelectedId(audioProcessor.getModeParamValue() + 1);
    addAndMakeVisible(evaluationModeMenu);

    setSize (400, 300);

    audioProcessor.getGenerator().setCallback([this](juce::String msg) { console.setText(msg); });
}

ByteToneAudioProcessorEditor::~ByteToneAudioProcessorEditor()
{
}

void ByteToneAudioProcessorEditor::sourceSampleRateChanged()
{
    evaluateCode();
}

void ByteToneAudioProcessorEditor::evaluateModeChanged(int mode)
{
    audioProcessor.setModeParamValue(mode);
    evaluateCode();
}

void ByteToneAudioProcessorEditor::evaluateCode()
{
    console.setText("Processing...");
    audioProcessor.getGenerator().generate();
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
