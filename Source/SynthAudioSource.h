/*
  ==============================================================================

    SynthAudioSource.h
    Created: 14 Nov 2020 5:23:13pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>

struct Sound : public juce::SynthesiserSound
{
    Sound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class ByteToneAudioProcessor;

struct SynthVoice : public juce::SynthesiserVoice
{
    SynthVoice(ByteToneAudioProcessor& proc);

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    ByteToneAudioProcessor& processor;

    float leftGain;
    float rightGain;
    double positionInSource;
    double ratio;

    juce::ADSR adsr;
};

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState, ByteToneAudioProcessor& proc, int numVoices = 8) : keyboardState(keyState)
    {
        for (auto i = 0; i < numVoices; ++i)
            synth.addVoice(new SynthVoice(proc));

        synth.addSound(new Sound());
    }

    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }

    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void releaseResources() override {}

    void renderNextBlock(const juce::AudioSourceChannelInfo& bufferToFill, juce::MidiBuffer& incomingMidi)
    {
        bufferToFill.clearActiveBufferRegion();

        keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
    }

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
};
