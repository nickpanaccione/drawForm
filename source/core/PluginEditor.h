#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../ui/WavetableEditor.h"

class DrawFormAudioProcessorEditor  : public juce::AudioProcessorEditor {
public:
  DrawFormAudioProcessorEditor (DrawFormAudioProcessor&);
  ~DrawFormAudioProcessorEditor() override;

  void paint (juce::Graphics&) override;
  void resized() override;

private:
  DrawFormAudioProcessor& audioProcessor;
  WavetableEditor wavetableEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrawFormAudioProcessorEditor)
};
