#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../ui/WavetableEditor.h"

class DrawFormAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener {
public:
  DrawFormAudioProcessorEditor (DrawFormAudioProcessor&);
  ~DrawFormAudioProcessorEditor() override;

  void paint (juce::Graphics&) override;
  void resized() override;

private:
  void sliderValueChanged(juce::Slider* slider) override;
  void updateEnvelope();

  DrawFormAudioProcessor& audioProcessor;
  WavetableEditor wavetableEditor;

  juce::Slider attackSlider;
  juce::Slider decaySlider;
  juce::Slider sustainSlider;
  juce::Slider releaseSlider;

  juce::Label attackLabel;
  juce::Label decayLabel;
  juce::Label sustainLabel;
  juce::Label releaseLabel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrawFormAudioProcessorEditor)
};
