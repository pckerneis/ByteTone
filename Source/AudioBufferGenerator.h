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
    typedef std::function<void(juce::String)> CallbackFunction;

    AudioBufferGenerator(ByteToneAudioProcessor& p);
    ~AudioBufferGenerator();

    void setCallback(CallbackFunction cb) { callback = cb; }
    void generate() { shouldGenerate = true; }

private:
    void checkForBuffersToFree();
    void checkForCodeToEvaluate();

    juce::String evaluateCode();
    juce::AudioSampleBuffer generateFromText(juce::String text, int lengthInSamples);
    ReferenceCountedBuffer::Ptr resampleBuffer(const juce::String name, const juce::AudioSampleBuffer& buffer, int sourceSampleRate);

    float integerToSample(int integer);
    
    void run() override;

    ByteToneAudioProcessor& audioProcessor;
    Interpreter interpreter;

    std::atomic<bool> shouldGenerate;
    std::function<void(juce::String)> callback;

    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;
};