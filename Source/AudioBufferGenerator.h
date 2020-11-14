/*
  ==============================================================================

    AudioBufferGenerator.h
    Created: 14 Nov 2020 9:18:02am
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Interpreter.h"
#include "ReferenceCountedBuffer.h"

class ByteToneAudioProcessor;

class AudioBufferGenerator : public juce::Thread
{
    enum EvaluationMode
    {
        BYTE    = 0,
        FLOAT   = 1,
    };

public:
    AudioBufferGenerator(ByteToneAudioProcessor& p);
    ~AudioBufferGenerator();

    void checkForBuffersToFree();
    juce::String evaluateCode();
    juce::AudioSampleBuffer generateFromText(juce::String text, int lengthInSamples);
    ReferenceCountedBuffer::Ptr resampleBuffer(const juce::String name, const juce::AudioSampleBuffer& buffer, int sourceSampleRate);

    float integerToSample(int integer);
    
    void run() override;
private:
    ByteToneAudioProcessor& audioProcessor;
    Interpreter interpreter;

    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;
};