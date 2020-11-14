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

    runButton.setButtonText("Compile");
    runButton.onClick = [this, &p] { 
        p.setCurrentCode(textEditor.getText());
        evaluateCode();
    };
    addAndMakeVisible(runButton);

    sampleRateLabel.setText("sr:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(sampleRateLabel);

    sampleRateSlider.setSliderStyle(Slider::LinearBar);
    sampleRateSlider.setTextValueSuffix("Hz");
    sampleRateAttachment.reset (new SliderAttachment (p.getParameters(), "sampleRate", sampleRateSlider));
    addAndMakeVisible(sampleRateSlider);

    gainLabel.setText("vol:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(gainLabel);

    gainSlider.setSliderStyle(Slider::LinearBar);
    gainAttachment.reset(new SliderAttachment(p.getParameters(), "gain", gainSlider));
    addAndMakeVisible(gainSlider);

    modeLabel.setText("mode:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(modeLabel);

    modeComboBox.addItem("byte", 1);
    modeComboBox.addItem("float", 2);
    modeAttachment.reset(new ComboBoxAttachment(p.getParameters(), "mode", modeComboBox));
    addAndMakeVisible(modeComboBox);

    setSize (400, 300);

    audioProcessor.getGenerator().setCallback([this](juce::String msg) { console.setText(msg); });
}

ByteToneAudioProcessorEditor::~ByteToneAudioProcessorEditor()
{
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
    g.setFont (lf.getDefaultFontHeight());
    const juce::String text = juce::String(ProjectInfo::projectName);
    g.drawFittedText (text, getLocalBounds().removeFromTop(28).withLeft(6), juce::Justification::left, 1);
}

void ByteToneAudioProcessorEditor::resized()
{
    const int lineHeight = 28;
    const int consoleHeight = lineHeight;
    const int buttonWidth = 80;
    const int comboWidth = 80;

    const int charW = 12;

    auto r = getLocalBounds();
    auto firstLine = r.removeFromTop(lineHeight);
    runButton.setBounds(firstLine.removeFromRight(buttonWidth));

    auto secondLine = r.removeFromBottom(lineHeight);
    sampleRateLabel.setBounds(secondLine.removeFromLeft(charW * 3));
    sampleRateSlider.setBounds(secondLine.removeFromLeft(comboWidth));
    modeLabel.setBounds(secondLine.removeFromLeft(charW * 5));
    modeComboBox.setBounds(secondLine.removeFromLeft(comboWidth));
    gainLabel.setBounds(secondLine.removeFromLeft(charW * 4));
    gainSlider.setBounds(secondLine.removeFromLeft(comboWidth));

    console.setBounds(r.removeFromBottom(consoleHeight));

    textEditor.setBounds(r);
}
