/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DrawFormAudioProcessor::DrawFormAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

DrawFormAudioProcessor::~DrawFormAudioProcessor()
{
}

//==============================================================================
const juce::String DrawFormAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DrawFormAudioProcessor::acceptsMidi() const
{
    return true;
}

bool DrawFormAudioProcessor::producesMidi() const
{
    return false;
}

bool DrawFormAudioProcessor::isMidiEffect() const
{
    return false;
}

double DrawFormAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DrawFormAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DrawFormAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DrawFormAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DrawFormAudioProcessor::getProgramName (int index)
{
    return {};
}

void DrawFormAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DrawFormAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synthEngine.setSampleRate(sampleRate);
}

void DrawFormAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DrawFormAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void DrawFormAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        if (message.isNoteOn())
            synthEngine.noteOn(message.getNoteNumber(), message.getFloatVelocity());
        else if (message.isNoteOff())
            synthEngine.noteOff(message.getNoteNumber());
    }

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float output = synthEngine.process();
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample(channel, sample, output);
    }
}

//==============================================================================
bool DrawFormAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DrawFormAudioProcessor::createEditor()
{
    return new DrawFormAudioProcessorEditor (*this);
}

//==============================================================================
void DrawFormAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DrawFormAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrawFormAudioProcessor();
}
