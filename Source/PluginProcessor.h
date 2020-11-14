/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ReferenceCountedBuffer.h"
#include "AudioBufferGenerator.h"

//==============================================================================
/**
*/
class ByteToneAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ByteToneAudioProcessor();
    ~ByteToneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setCurrentBuffer(ReferenceCountedBuffer::Ptr buffer) { currentBuffer = buffer; }

    int getSampleRateParamValue() const { return *sampleRate; }
    void setSampleRateParamValue(int newValue) const { *sampleRate = newValue; }
    float getGainParamValue() const { return *gain; }
    void setGainParamValue(float newValue) const { *gain = newValue; }
    int getModeParamValue() const { return *mode; }
    void setModeParamValue(int newMode) const { *mode = newMode; }

    AudioBufferGenerator& getGenerator() { return generator; }
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

private:
    AudioBufferGenerator generator;
    ReferenceCountedBuffer::Ptr currentBuffer;

    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* sampleRate = nullptr;
    std::atomic<float>* mode       = nullptr;
    std::atomic<float>* gain       = nullptr;

    float previousGain;
    double gainRampTime = 0.01;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessor)
};
