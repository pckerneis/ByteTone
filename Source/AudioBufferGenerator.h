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
    void updateSourceCode() { shouldUpdate = true; }

private:
    void checkForBuffersToFree();
    void checkForCodeToEvaluate();

    juce::String evaluateCode();
    ReferenceCountedBuffer::Ptr generateFromText(juce::String text, int lengthInSamples);

    float integerToSample(int integer);
    
    void run() override;

    ByteToneAudioProcessor& audioProcessor;
    Interpreter interpreter;

    std::atomic<bool> shouldUpdate = false;
    std::function<void(juce::String)> callback;

    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;
};