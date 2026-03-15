#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../ui/WavetableEditor.h"

class DrawFormAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::ComboBox::Listener {
public:
  DrawFormAudioProcessorEditor (DrawFormAudioProcessor&);
  ~DrawFormAudioProcessorEditor() override;

  void paint (juce::Graphics&) override;
  void resized() override;

private:
  void sliderValueChanged(juce::Slider* slider) override;
  void comboBoxChanged(juce::ComboBox* comboBox) override;
  void updateEnvelope();

  DrawFormAudioProcessor& audioProcessor;
  WavetableEditor wavetableEditor;

  juce::Slider attackSlider;
  juce::Slider decaySlider;
  juce::Slider sustainSlider;
  juce::Slider releaseSlider;
  juce::Slider morphSlider;

  juce::Label attackLabel;
  juce::Label decayLabel;
  juce::Label sustainLabel;
  juce::Label releaseLabel;
  juce::Label morphLabel;

  juce::ComboBox frameSelector;
  juce::Label frameLabel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrawFormAudioProcessorEditor)
};
