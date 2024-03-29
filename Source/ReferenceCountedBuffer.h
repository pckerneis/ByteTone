/*
  ==============================================================================

    ReferenceCountedBuffer.h
    Created: 11 Nov 2020 5:49:46pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ReferenceCountedBuffer : public juce::ReferenceCountedObject
{
public:
    typedef juce::ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

    ReferenceCountedBuffer(const juce::String& nameToUse,
        int numChannels,
        int numSamples)
        : name(nameToUse),
        buffer(numChannels, numSamples)
    {
        DBG(juce::String("Buffer named '") + name + "' constructed. numChannels = " + juce::String(numChannels) + ", numSamples = " + juce::String(numSamples));
    }

    ~ReferenceCountedBuffer()
    {
        DBG(juce::String("Buffer named '") + name + "' destroyed");
    }

    juce::AudioSampleBuffer* getAudioSampleBuffer()
    {
        return &buffer;
    }

private:
    juce::String name;
    juce::AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReferenceCountedBuffer)
};
