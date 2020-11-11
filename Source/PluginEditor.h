/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Interpreter.h"

//==============================================================================
/**
*/
class ByteToneAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Thread
{
public:
    ByteToneAudioProcessorEditor (ByteToneAudioProcessor&);
    ~ByteToneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void run() override;

private:
    void checkForBuffersToFree();

    void evaluateText();
    juce::AudioSampleBuffer generateFromText(juce::String text, int lengthInSamples);
    ReferenceCountedBuffer::Ptr resampleBuffer(const juce::String name, const juce::AudioSampleBuffer& buffer, int sourceSampleRate);
    float integerToSample(int integer);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ByteToneAudioProcessor& audioProcessor;

    juce::TextEditor textEditor;
    juce::TextButton runButton;

    Interpreter interpreter;
    
    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessorEditor)
};
