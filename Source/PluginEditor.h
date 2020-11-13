/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Interpreter.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class ByteToneAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Thread
{
    enum EvaluationMode
    {
        BYTE,
        FLOAT,
    };

public:
    ByteToneAudioProcessorEditor (ByteToneAudioProcessor&);
    ~ByteToneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void run() override;

private:
    void sourceSampleRateChanged();
    void evaluateModeChanged();
    void checkForBuffersToFree();
    void evaluateCode();
    juce::AudioSampleBuffer generateFromText(juce::String text, int lengthInSamples);
    ReferenceCountedBuffer::Ptr resampleBuffer(const juce::String name, const juce::AudioSampleBuffer& buffer, int sourceSampleRate);
    float integerToSample(int integer);

    ByteToneAudioProcessor& audioProcessor;

    juce::ComboBox sourceSampleRateMenu;
    juce::ComboBox evaluationModeMenu;
    juce::TextEditor textEditor;
    juce::TextButton runButton;
    juce::TextEditor console;

    Interpreter interpreter;
    
    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;

    CustomLookAndFeel lf;

    juce::String code;
    int sourceSampleRate;

    EvaluationMode evaluationMode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessorEditor)
};
