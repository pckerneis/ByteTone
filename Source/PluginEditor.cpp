#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByteToneAudioProcessorEditor::ByteToneAudioProcessorEditor (ByteToneAudioProcessor& p)
    : AudioProcessorEditor (&p),
    audioProcessor(p)
{
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

    settingsButton.setButtonText("Settings");
    settingsButton.onClick = [this, &p] {
        openSettings();
    };
    addAndMakeVisible(settingsButton);

    runButton.setButtonText("Compile");
    runButton.onClick = [this, &p] { 
        p.setCurrentCode(textEditor.getText());
        evaluateCode();
    };
    addAndMakeVisible(runButton);

    startStopButton.setButtonText("Playing");
    startStopButton.setToggleable(true);
    playingAttachment.reset(new ButtonAttachment(p.getParameters(), "playing", startStopButton));
    addAndMakeVisible(startStopButton);

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

    setResizable(true, true);
    setResizeLimits(500, 200, 3000, 3000);
    setSize (500, 400);

    audioProcessor.getGenerator().setCallback([this](juce::String msg) { console.setText(msg); });
}

ByteToneAudioProcessorEditor::~ByteToneAudioProcessorEditor()
{
}

#include "StandaloneWindow.h"

void ByteToneAudioProcessorEditor::openSettings()
{
    if (JUCEApplication::isStandaloneApp())
    {
        if (auto app = dynamic_cast<StandaloneFilterApp*>(JUCEApplication::getInstance()))
            app->openSettings();
    }
}

void ByteToneAudioProcessorEditor::evaluateCode()
{
    console.setText("Processing...");
    audioProcessor.getGenerator().generate();
}

void ByteToneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (lf.backgroundColour);

    g.setColour (lf.textColour);
    g.setFont (lf.getDefaultFontHeight());
    const juce::String text = juce::String(ProjectInfo::projectName);
    g.drawFittedText (text, getLocalBounds().removeFromTop(28).withLeft(6), juce::Justification::left, 1);
}

void ByteToneAudioProcessorEditor::resized()
{
    const int lineHeight = 28;
    const int consoleHeight = lineHeight;
    const int buttonWidth = 80;
    const int srWidth = 70;
    const int comboWidth = 80;
    const int sliderWidth = 50;
    const int keyboardHeight = 80;

    const int charW = 12;

    auto r = getLocalBounds();
    auto firstLine = r.removeFromTop(lineHeight);

    if (JUCEApplication::isStandaloneApp())
        settingsButton.setBounds(firstLine.removeFromRight(buttonWidth));

    auto secondLine = r.removeFromBottom(lineHeight);
    sampleRateLabel.setBounds(secondLine.removeFromLeft(charW * 3));
    sampleRateSlider.setBounds(secondLine.removeFromLeft(srWidth));
    modeLabel.setBounds(secondLine.removeFromLeft(charW * 4));
    modeComboBox.setBounds(secondLine.removeFromLeft(comboWidth));
    gainLabel.setBounds(secondLine.removeFromLeft(charW * 3));
    gainSlider.setBounds(secondLine.removeFromLeft(sliderWidth));

    startStopButton.setBounds(secondLine.removeFromRight(buttonWidth));
    runButton.setBounds(secondLine.removeFromRight(buttonWidth));

    console.setBounds(r.removeFromBottom(consoleHeight));

    textEditor.setBounds(r);
}
