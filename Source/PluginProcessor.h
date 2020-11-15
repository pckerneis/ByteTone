#pragma once

#include <JuceHeader.h>
#include "ReferenceCountedBuffer.h"
#include "AudioBufferGenerator.h"
#include "SynthAudioSource.h"

//==============================================================================
/**
*/
class ByteToneAudioProcessor  : public juce::AudioProcessor
{
    juce::String defaultProgram = "t * ((t>>12 | t>>9) & (t>>6) & 50)";

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

    //==============================================================================
    void setCurrentBuffer(ReferenceCountedBuffer::Ptr buffer) { currentBuffer = buffer; }

    int getSampleRateParamValue() const { return *sampleRate; }
    float getGainParamValue() const { return *gain; }
    int getModeParamValue() const { return *mode; }
    int getNoteParamValue() const { return *note; }

    juce::String getCurrentCode() const { return getCodeValueTree().getProperty("code"); }
    void setCurrentCode(juce::String code) { getCodeValueTree().setProperty("code", code, parameters.undoManager); }

    AudioBufferGenerator& getGenerator() { return generator; }
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }
    bool addingFromMidiInput() const { return isAddingFromMidiInput; }
    ReferenceCountedBuffer::Ptr getCurrentBuffer() const { return currentBuffer; }

private:
    juce::ValueTree getCodeValueTree() { return parameters.state.getChildWithName("CODE"); }
    juce::ValueTree getCodeValueTree() const { return parameters.state.getChildWithName("CODE"); }

    AudioBufferGenerator generator;
    ReferenceCountedBuffer::Ptr currentBuffer;

    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* sampleRate = nullptr;
    std::atomic<float>* mode       = nullptr;
    std::atomic<float>* gain       = nullptr;
    std::atomic<float>* note       = nullptr;

    float previousGain;
    double gainRampTime = 0.01;

    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthAudioSource;

    bool isAddingFromMidiInput;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByteToneAudioProcessor)
};
