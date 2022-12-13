/*
  ==============================================================================

    Waveform.h
    Created: 11 Dec 2022 6:12:15pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Waveform  : public juce::Component, juce::Timer
{
public:
    Waveform(ByteToneAudioProcessor& p):
        audioProcessor(p),
        audioVisualiserComponent(1)
    {
        addAndMakeVisible(audioVisualiserComponent);

        audioVisualiserComponent.setBufferSize(256);
        audioVisualiserComponent.setRepaintRate(10);

        startTimer(100);
    }

    ~Waveform() override
    {
    }

    void paint (juce::Graphics& g) override
    {
    }

    void resized() override
    {
        audioVisualiserComponent.setBounds(getLocalBounds());
    }

    // TODO dedup
    float integerToSample(int integer)
    {
        const int max = 255;
        return juce::jmap((float)(integer & max), 0.0f, (float)max, -1.0f, 1.0f);
    }

    void timerCallback() override
    {
        int numSamples = audioProcessor.getSampleRateParamValue() / 10;

        AudioBuffer<float> buffer (1, numSamples);

        if (audioProcessor.isPlaying())
        {
            Interpreter interpreter;
            bool floatMode = audioProcessor.getModeParamValue() == ByteToneAudioProcessor::FLOAT;

            Array<Var> values = interpreter.generateRange(audioProcessor.getCurrentCode(),
                (int)audioProcessor.getPositionInSource(),
                numSamples);

            for (int i = 0; i < numSamples; ++i)
            {
                float value = floatMode ? values[i].coercedToDouble() : integerToSample(values[i].coercedToDouble());
                buffer.setSample(0, i, juce::jlimit(-1.0f, 1.0f, value));
            }
        }

        audioVisualiserComponent.pushBuffer(buffer);
    }

private:
    ByteToneAudioProcessor& audioProcessor;
    juce::AudioVisualiserComponent audioVisualiserComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Waveform)
};
