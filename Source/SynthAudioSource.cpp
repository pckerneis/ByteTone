/*
  ==============================================================================

    SynthAudioSource.cpp
    Created: 14 Nov 2020 5:23:13pm
    Author:  Pierre

  ==============================================================================
*/

#include "SynthAudioSource.h"

SynthVoice::SynthVoice(ByteToneAudioProcessor& proc) : processor(proc), position(0)
{
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<Sound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    position = 0;
    isOn = true;
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    isOn = false;
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

#include "ReferenceCountedBuffer.h"
#include "PluginProcessor.h"

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    ReferenceCountedBuffer::Ptr retainedCurrentBuffer(processor.getCurrentBuffer());

    if (retainedCurrentBuffer == nullptr || ! isOn)
    {
        return;
    }

    auto* currentAudioSampleBuffer = retainedCurrentBuffer->getAudioSampleBuffer();

    auto numInputChannels = currentAudioSampleBuffer->getNumChannels();
    auto numOutputChannels = outputBuffer.getNumChannels();

    int outputSamplesRemaining = numSamples;
    auto outputSamplesOffset = startSample;

    while (outputSamplesRemaining > 0)
    {
        auto bufferSamplesRemaining = currentAudioSampleBuffer->getNumSamples() - position;
        auto samplesThisTime = juce::jmin(outputSamplesRemaining, bufferSamplesRemaining);

        for (auto channel = 0; channel < numOutputChannels; ++channel)
        {
            outputBuffer.addFrom(channel,
                outputSamplesOffset,
                *currentAudioSampleBuffer,
                channel % numInputChannels,
                position,
                samplesThisTime);
        }

        outputSamplesRemaining -= samplesThisTime;
        outputSamplesOffset += samplesThisTime;
        position += samplesThisTime;

        if (position == currentAudioSampleBuffer->getNumSamples())
            position = 0;
    }
}
