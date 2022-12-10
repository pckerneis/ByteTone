/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/**
*/
class ByteToneAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::MidiKeyboardState::Listener
{
public:
    ByteToneAudioProcessorEditor (ByteToneAudioProcessor&);
    ~ByteToneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    virtual void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    virtual void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    void openSettings();

    void evaluateCode();

    ByteToneAudioProcessor& audioProcessor;

    juce::Slider sampleRateSlider;
    std::unique_ptr<SliderAttachment> sampleRateAttachment;

    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ComboBox modeComboBox;
    std::unique_ptr<ComboBoxAttachment> modeAttachment;

    juce::Slider noteSlider;
    std::unique_ptr<SliderAttachment> noteAttachment;

    juce::ToggleButton startStopButton;
    std::unique_ptr<ButtonAttachment> playingAttachment;

    juce::Label sampleRateLabel;
    juce::Label gainLabel;
    juce::Label modeLabel;
    juce::Label noteLabel;

    juce::TextButton settingsButton;
    juce::TextButton runButton;
    juce::TextEditor textEditor;
    juce::TextEditor console;

    juce::MidiKeyboardState& keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;

    CustomLookAndFeel lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessorEditor)
};
