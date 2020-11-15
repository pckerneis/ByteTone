/*
  ==============================================================================

    SynthAudioSource.cpp
    Created: 14 Nov 2020 5:23:13pm
    Author:  Pierre

  ==============================================================================
*/

#include "SynthAudioSource.h"
#include "ReferenceCountedBuffer.h"
#include "PluginProcessor.h"

SynthVoice::SynthVoice(ByteToneAudioProcessor& proc) : processor(proc), positionInSource(0), leftGain(0.0f), rightGain(0.0f), ratio(0.0)
{
    adsr.setParameters({ 0.005f, 0.0f, 1.0f, 0.01f });
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<Sound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    positionInSource = 0;

    ratio = pow(2.0, ((double)midiNoteNumber - processor.getNoteParamValue()) / 12.0)
        * processor.getSampleRateParamValue() / getSampleRate();

    leftGain = velocity;
    rightGain = velocity;

    adsr.setSampleRate(processor.getSampleRate());

    adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    ReferenceCountedBuffer::Ptr retainedCurrentBuffer(processor.getCurrentBuffer());

    if (retainedCurrentBuffer == nullptr)
    {
        return;
    }

    auto* sourceBuffer = retainedCurrentBuffer->getAudioSampleBuffer();

    const float* const inL = sourceBuffer->getReadPointer(0);
    const float* const inR = sourceBuffer->getNumChannels() > 1 ? sourceBuffer->getReadPointer(1) : nullptr;

    float* outL = outputBuffer.getWritePointer(0, startSample);
    float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

    while (--numSamples >= 0)
    {
        auto pos = (int)positionInSource;
        auto alpha = (float)(positionInSource - pos);
        auto invAlpha = 1.0f - alpha;

        // just using a very simple linear interpolation here..
        float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
        float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
            : l;

        auto envelopeValue = adsr.getNextSample();

        l *= envelopeValue * leftGain;
        r *= envelopeValue * rightGain;

        if (outR != nullptr)
        {
            *outL++ += l;
            *outR++ += r;
        }
        else
        {
            *outL++ += (l + r) * 0.5f;
        }

        positionInSource += ratio;

        if (positionInSource > sourceBuffer->getNumSamples())
        {
            positionInSource = 0.0;
        }
    }
}
