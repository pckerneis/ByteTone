/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

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

private:
    void sourceSampleRateChanged();
    void evaluateModeChanged(int mode);

    void evaluateCode();

    ByteToneAudioProcessor& audioProcessor;

    juce::Slider sampleRateSlider;
    std::unique_ptr<SliderAttachment> sampleRateAttachment;
    juce::ComboBox evaluationModeMenu;

    juce::TextEditor textEditor;
    juce::TextButton runButton;
    juce::TextEditor console;

    CustomLookAndFeel lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessorEditor)
};
