/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByteToneAudioProcessor::ByteToneAudioProcessor()
     : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    generator(*this),
    previousGain(0),
    parameters (*this, nullptr, juce::Identifier("ByteToneAudioProcessor"),
        {
            std::make_unique<juce::AudioParameterFloat>("gain",
                                                        "Gain",
                                                        0.0f,
                                                        1.0f,
                                                        0.5f),

            std::make_unique<juce::AudioParameterInt>("sampleRate",
                                                      "Sample rate",
                                                      1000,
                                                      100000,
                                                      8000),

            std::make_unique<juce::AudioParameterChoice>("mode",
                                                         "Mode",
                                                         juce::StringArray("byte", "float"),
                                                         0),

            std::make_unique<juce::AudioParameterInt>("note",
                                                      "Note",
                                                      0,
                                                      127,
                                                      69)
        }),
    synthAudioSource(keyboardState, *this),
    isAddingFromMidiInput(false)
{
    gain = parameters.getRawParameterValue("gain");
    sampleRate = parameters.getRawParameterValue("sampleRate");
    mode = parameters.getRawParameterValue("mode");
    note = parameters.getRawParameterValue("note");
}

ByteToneAudioProcessor::~ByteToneAudioProcessor()
{
}

//==============================================================================
const juce::String ByteToneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ByteToneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ByteToneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ByteToneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ByteToneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ByteToneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ByteToneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ByteToneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ByteToneAudioProcessor::getProgramName (int index)
{
    return {};
}

void ByteToneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ByteToneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    previousGain = *gain;
    synthAudioSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void ByteToneAudioProcessor::releaseResources()
{
    synthAudioSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ByteToneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ByteToneAudioProcessor::processBlock(juce::AudioBuffer<float>& bufferToFill, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamplesToFill = bufferToFill.getNumSamples();

    {
        const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
        keyboardState.processNextMidiBuffer(midiMessages, 0, numSamplesToFill, true);
    }

    for (auto i = 0; i < totalNumOutputChannels; ++i)
        bufferToFill.clear(i, 0, numSamplesToFill);

    synthAudioSource.renderNextBlock(juce::AudioSourceChannelInfo(&bufferToFill, 0, numSamplesToFill), midiMessages);

    float currentGain = *gain;

    if (currentGain == previousGain)
    {
        bufferToFill.applyGain(currentGain);
    }
    else
    {
        const int rampSamples = juce::jmin((int)(gainRampTime / getSampleRate()), numSamplesToFill);
        bufferToFill.applyGainRamp(0, rampSamples, previousGain, currentGain);

        if (rampSamples < numSamplesToFill)
            bufferToFill.applyGain(rampSamples, numSamplesToFill - rampSamples, currentGain);

        previousGain = currentGain;
    }
}

//==============================================================================
bool ByteToneAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ByteToneAudioProcessor::createEditor()
{
    auto editor = new ByteToneAudioProcessorEditor (*this);

    if (wrapperType == wrapperType_Standalone)
    {
        if (TopLevelWindow::getNumTopLevelWindows() == 1)
        {
            TopLevelWindow* w = TopLevelWindow::getTopLevelWindow(0);
            w->setUsingNativeTitleBar(true);
        }
    }
    return editor;
}

//==============================================================================
void ByteToneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ByteToneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    auto codeValueTree = parameters.state.getOrCreateChildWithName("CODE", parameters.undoManager);

    if (getCurrentCode().isEmpty())
    {
        setCurrentCode(defaultProgram);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ByteToneAudioProcessor();
}
