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

juce::String AudioBufferGenerator::evaluateCode()
{
    const juce::String code(audioProcessor.getCurrentCode());

    if (code.trim().isEmpty())
    {
        return "Source code is empty.";
    }

    try
    {
        const int sr = audioProcessor.getSampleRateParamValue();
        const int lengthInSamples = sr * 30;
        ReferenceCountedBuffer::Ptr newBuffer = generateFromText(code, lengthInSamples);
        audioProcessor.setCurrentBuffer(newBuffer);

        return "Done.";
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

ReferenceCountedBuffer::Ptr AudioBufferGenerator::generateFromText(juce::String text, int lengthInSamples)
{
    const juce::Array<Var> result = interpreter.generate(text, lengthInSamples);

    ReferenceCountedBuffer::Ptr buffer(new ReferenceCountedBuffer(text, 2, lengthInSamples));
    buffers.add(buffer);

    int destSample = 0;
    bool floatMode = audioProcessor.getModeParamValue() == EvaluationMode::FLOAT;

    for (const Var r : result)
    {
        float sample = floatMode ? (float)r : integerToSample((int)r);
        buffer->getAudioSampleBuffer()->setSample(0, destSample, sample);
        buffer->getAudioSampleBuffer()->setSample(1, destSample, sample);
        destSample++;
    }

    return buffer;
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
        checkForCodeToEvaluate();
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

void AudioBufferGenerator::checkForCodeToEvaluate()
{
    if (shouldUpdate)
    {
        juce::String result = evaluateCode();
        juce::MessageManager::callAsync([=]() { callback(result); });
        shouldUpdate = false;
    }
}
