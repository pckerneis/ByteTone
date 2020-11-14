/*
  ==============================================================================

    AudioBufferGenerator.cpp
    Created: 14 Nov 2020 9:18:02am
    Author:  Pierre

  ==============================================================================
*/

#include "AudioBufferGenerator.h"
#include "PluginProcessor.h"

AudioBufferGenerator::AudioBufferGenerator(ByteToneAudioProcessor& p) : Thread("AudioBufferGenerator"), audioProcessor(p)
{
    startThread();
}

AudioBufferGenerator::~AudioBufferGenerator()
{
    stopThread(1000);
}

juce::String AudioBufferGenerator::evaluateCode(juce::String code)
{
    if (code.trim().isEmpty())
    {
        return "Source code is empty.";
    }

    try
    {
        const int sr = audioProcessor.getSampleRateParamValue();
        const int lengthInSamples = sr * 30;
        const juce::AudioSampleBuffer tempBuffer = generateFromText(code, lengthInSamples);
        ReferenceCountedBuffer::Ptr newBuffer = resampleBuffer(code, tempBuffer, sr);
        audioProcessor.setCurrentBuffer(newBuffer);
        buffers.add(newBuffer);

        return juce::String();
    }
    catch (ParseError& exception)
    {
        return exception.getMessage();
    }
    catch (ScanError& exception)
    {
        return exception.getMessage();
    }
}

juce::AudioSampleBuffer AudioBufferGenerator::generateFromText(juce::String text, int lengthInSamples)
{
    const juce::Array<Var> result = interpreter.generate(text, lengthInSamples);

    int destSample = 0;
    juce::AudioSampleBuffer tempBuffer(2, lengthInSamples);
    bool floatMode = audioProcessor.getModeParamValue() == EvaluationMode::FLOAT;

    for (const Var r : result)
    {
        float sample = floatMode ? (float)r : integerToSample((int)r);
        tempBuffer.setSample(0, destSample, sample);
        tempBuffer.setSample(1, destSample, sample);
        destSample++;
    }

    return tempBuffer;
}

ReferenceCountedBuffer::Ptr AudioBufferGenerator::resampleBuffer(const juce::String name,
    const juce::AudioSampleBuffer& buffer, int sourceSampleRate)
{
    double ratio = sourceSampleRate / audioProcessor.getSampleRate();

    ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(name,
        audioProcessor.getNumOutputChannels(), buffer.getNumSamples() / ratio);

    float** outputs = newBuffer->getAudioSampleBuffer()->getArrayOfWritePointers();
    const float** inputs = buffer.getArrayOfReadPointers();

    for (int c = 0; c < newBuffer->getAudioSampleBuffer()->getNumChannels(); c++)
    {
        juce::LagrangeInterpolator resampler;
        resampler.process(ratio, inputs[c], outputs[c], newBuffer->getAudioSampleBuffer()->getNumSamples());
    }

    return newBuffer;
}

float AudioBufferGenerator::integerToSample(int integer)
{
    const int max = 255;
    return juce::jmap((float)(integer & max), 0.0f, (float)max, -1.0f, 1.0f);
}

void AudioBufferGenerator::run()
{
    while (!threadShouldExit())
    {
        checkForBuffersToFree();
        wait(500);
    }
}

void AudioBufferGenerator::checkForBuffersToFree()
{
    for (auto i = buffers.size(); --i >= 0;)
    {
        ReferenceCountedBuffer::Ptr buffer(buffers.getUnchecked(i));

        if (buffer->getReferenceCount() == 2)
            buffers.remove(i);
    }
}