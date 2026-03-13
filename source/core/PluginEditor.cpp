#include "PluginProcessor.h"
#include "PluginEditor.h"

DrawFormAudioProcessorEditor::DrawFormAudioProcessorEditor(DrawFormAudioProcessor& p)
  : AudioProcessorEditor(&p),
    audioProcessor(p),
    wavetableEditor(p.getSynthEngine().getWavetable()) {
  addAndMakeVisible(wavetableEditor);
  setSize(600, 300);
}

DrawFormAudioProcessorEditor::~DrawFormAudioProcessorEditor() {
}

void DrawFormAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::darkgrey);
}

void DrawFormAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds().reduced(20);
  wavetableEditor.setBounds(bounds);
}
