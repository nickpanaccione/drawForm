#include "PluginProcessor.h"
#include "PluginEditor.h"

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

DrawFormAudioProcessor::~DrawFormAudioProcessor() {
}

const juce::String DrawFormAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool DrawFormAudioProcessor::acceptsMidi() const {
  return true;
}

bool DrawFormAudioProcessor::producesMidi() const {
  return false;
}

bool DrawFormAudioProcessor::isMidiEffect() const {
  return false;
}

double DrawFormAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int DrawFormAudioProcessor::getNumPrograms() {
  return 1;   
}

int DrawFormAudioProcessor::getCurrentProgram() {
  return 0;
}

void DrawFormAudioProcessor::setCurrentProgram (int index) {
}

const juce::String DrawFormAudioProcessor::getProgramName (int index) {
  return {};
}

void DrawFormAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

void DrawFormAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
  synthEngine.setSampleRate(sampleRate);
}

void DrawFormAudioProcessor::releaseResources() {
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DrawFormAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) {
      return false;
    }
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) {
      return false;
    }
   #endif

    return true;
  #endif
}
#endif

void DrawFormAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  buffer.clear();

  for (const auto metadata : midiMessages) {
    auto message = metadata.getMessage();
    if (message.isNoteOn()) {
      synthEngine.noteOn(message.getNoteNumber(), message.getFloatVelocity());
    } else if (message.isNoteOff()) {
      synthEngine.noteOff(message.getNoteNumber());
    }
  }

  for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    float output = synthEngine.process();
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
      buffer.setSample(channel, sample, output);
    }
  }
}

bool DrawFormAudioProcessor::hasEditor() const {
  return true; 
}

juce::AudioProcessorEditor* DrawFormAudioProcessor::createEditor() {
  return new DrawFormAudioProcessorEditor (*this);
}

void DrawFormAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
}

void DrawFormAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new DrawFormAudioProcessor();
}
