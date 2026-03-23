#include "PluginProcessor.h"
#include "PluginEditor.h"

DrawFormAudioProcessorEditor::DrawFormAudioProcessorEditor (DrawFormAudioProcessor& p)
  : AudioProcessorEditor (&p), audioProcessor (p) {
    setSize (400, 300);
}

DrawFormAudioProcessorEditor::~DrawFormAudioProcessorEditor() {
}

void DrawFormAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DrawFormAudioProcessorEditor::resized() {
}
