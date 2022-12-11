/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "CodeEditor.h"
#include "Waveform.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/**
*/
class ByteToneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ByteToneAudioProcessorEditor (ByteToneAudioProcessor&);
    ~ByteToneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed(const KeyPress& key) override
    {
        if (key.getModifiers().isCommandDown()) {
            if (key.isKeyCurrentlyDown(key.returnKey))
            {
                startStopButton.setToggleState(!audioProcessor.isPlaying(), NotificationType::sendNotification);
                return true;
            }
            else if (key.isKeyCurrentlyDown(key.spaceKey))
            {
                updateAndCheckCode();
                return true;
            }
        }

        return false; // Key command not consumed
    }
private:
    void updateAndCheckCode();

    void openSettings();
    
    ApplicationCommandManager commandManager;

    ByteToneAudioProcessor& audioProcessor;

    juce::Slider sampleRateSlider;
    std::unique_ptr<SliderAttachment> sampleRateAttachment;

    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ComboBox modeComboBox;
    std::unique_ptr<ComboBoxAttachment> modeAttachment;

    juce::TextButton startStopButton;
    std::unique_ptr<ButtonAttachment> playingAttachment;

    juce::Label sampleRateLabel;
    juce::Label gainLabel;
    juce::Label modeLabel;

    juce::TextButton settingsButton;
    juce::TextButton runButton;
    CodeEditor textEditor;
    juce::TextEditor console;

    Waveform waveform;

    CustomLookAndFeel lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessorEditor)
};
