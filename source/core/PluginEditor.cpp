#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../dsp/NoiseOscillator.h"

DrawFormAudioProcessorEditor::DrawFormAudioProcessorEditor(DrawFormAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      wavetableEditor(p.getSynthEngine().getWavetable()) {
  addAndMakeVisible(wavetableEditor);

  auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& name,
                             double min, double max, double initial) {
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setRange(min, max, 0.001);
    slider.setValue(initial);
    slider.addListener(this);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
  };

  setupSlider(attackSlider, attackLabel, "Attack", 0.001, 2.0, 0.01);
  setupSlider(decaySlider, decayLabel, "Decay", 0.001, 2.0, 0.1);
  setupSlider(sustainSlider, sustainLabel, "Sustain", 0.0, 1.0, 0.7);
  setupSlider(releaseSlider, releaseLabel, "Release", 0.001, 3.0, 0.3);
  setupSlider(morphSlider, morphLabel, "Morph", 0.0, 1.0, 0.0);
  setupSlider(driftSlider, driftLabel, "Drift", 0.0, 1.0, 0.0);
  setupSlider(noiseSlider, noiseLabel, "Noise", 0.0, 1.0, 0.0);
  setupSlider(detuneSlider, detuneLabel, "Detune", -100.0, 100.0, 0.0);
  setupSlider(pitchBendRangeSlider, pitchBendRangeLabel, "PB Range", 1.0, 24.0, 2.0);
  setupSlider(arpRateSlider, arpRateLabel, "Arp Rate", 1.0, 20.0, 4.0);

  detuneSlider.setTextValueSuffix(" ct");
  pitchBendRangeSlider.setTextValueSuffix(" st");
  arpRateSlider.setTextValueSuffix(" Hz");

  noiseTypeLabel.setText("Noise Type", juce::dontSendNotification);
  noiseTypeLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(noiseTypeLabel);

  noiseTypeSelector.addItem("White", 1);
  noiseTypeSelector.addItem("Pink", 2);
  noiseTypeSelector.addItem("Brown", 3);
  noiseTypeSelector.setSelectedId(1);
  noiseTypeSelector.addListener(this);
  addAndMakeVisible(noiseTypeSelector);

  frameLabel.setText("Frame", juce::dontSendNotification);
  frameLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(frameLabel);

  int numFrames = p.getSynthEngine().getWavetable().getNumFrames();
  for (int i = 0; i < numFrames; ++i) {
    frameSelector.addItem("Frame " + juce::String(i + 1), i + 1);
  }
  frameSelector.setSelectedId(1);
  frameSelector.addListener(this);
  addAndMakeVisible(frameSelector);

  arpEnabledButton.setButtonText("Arp");
  arpEnabledButton.onClick = [this]() {
    audioProcessor.getSynthEngine().setArpeggiatorEnabled(arpEnabledButton.getToggleState());
  };
  addAndMakeVisible(arpEnabledButton);

  arpModeLabel.setText("Arp Mode", juce::dontSendNotification);
  arpModeLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(arpModeLabel);

  arpModeSelector.addItem("Up", 1);
  arpModeSelector.addItem("Down", 2);
  arpModeSelector.addItem("Up/Down", 3);
  arpModeSelector.addItem("Random", 4);
  arpModeSelector.setSelectedId(1);
  arpModeSelector.addListener(this);
  addAndMakeVisible(arpModeSelector);

  updateEnvelope();

  setSize(1000, 550);
}

DrawFormAudioProcessorEditor::~DrawFormAudioProcessorEditor() {
}

void DrawFormAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::darkgrey);
}

void DrawFormAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds().reduced(20);

  auto topArea = bounds.removeFromTop(30);
  frameLabel.setBounds(topArea.removeFromLeft(50));
  frameSelector.setBounds(topArea.removeFromLeft(120));

  topArea.removeFromLeft(20);
  noiseTypeLabel.setBounds(topArea.removeFromLeft(70));
  noiseTypeSelector.setBounds(topArea.removeFromLeft(100));

  topArea.removeFromLeft(20);
  arpEnabledButton.setBounds(topArea.removeFromLeft(50));
  arpModeLabel.setBounds(topArea.removeFromLeft(60));
  arpModeSelector.setBounds(topArea.removeFromLeft(100));

  bounds.removeFromTop(10);

  auto waveformArea = bounds.removeFromTop(200);
  wavetableEditor.setBounds(waveformArea);

  bounds.removeFromTop(20);

  auto sliderWidth = bounds.getWidth() / 10;
  auto sliderArea = bounds;

  auto setupArea = [&](juce::Slider& slider, juce::Label& label) {
    auto area = sliderArea.removeFromLeft(sliderWidth);
    label.setBounds(area.removeFromTop(20));
    slider.setBounds(area);
  };

  setupArea(attackSlider, attackLabel);
  setupArea(decaySlider, decayLabel);
  setupArea(sustainSlider, sustainLabel);
  setupArea(releaseSlider, releaseLabel);
  setupArea(morphSlider, morphLabel);
  setupArea(driftSlider, driftLabel);
  setupArea(noiseSlider, noiseLabel);
  setupArea(detuneSlider, detuneLabel);
  setupArea(pitchBendRangeSlider, pitchBendRangeLabel);
  setupArea(arpRateSlider, arpRateLabel);
}

void DrawFormAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
  if (slider == &morphSlider) {
    float morphValue = static_cast<float>(morphSlider.getValue());
    audioProcessor.getSynthEngine().setFramePosition(morphValue);
    wavetableEditor.setMorphPosition(morphValue);
  } else if (slider == &driftSlider) {
    audioProcessor.getSynthEngine().setDriftAmount(static_cast<float>(driftSlider.getValue()));
  } else if (slider == &noiseSlider) {
    audioProcessor.getSynthEngine().setNoiseLevel(static_cast<float>(noiseSlider.getValue()));
  } else if (slider == &detuneSlider) {
    audioProcessor.getSynthEngine().setDetune(static_cast<float>(detuneSlider.getValue()));
  } else if (slider == &pitchBendRangeSlider) {
    audioProcessor.getSynthEngine().setPitchBendRange(static_cast<float>(pitchBendRangeSlider.getValue()));
  } else if (slider == &arpRateSlider) {
    audioProcessor.getSynthEngine().setArpeggiatorRate(static_cast<float>(arpRateSlider.getValue()));
  } else {
    updateEnvelope();
  }
}

void DrawFormAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox) {
  if (comboBox == &frameSelector) {
    wavetableEditor.setCurrentFrame(frameSelector.getSelectedId() - 1);
  } else if (comboBox == &noiseTypeSelector) {
    auto type = static_cast<NoiseOscillator::NoiseType>(noiseTypeSelector.getSelectedId() - 1);
    audioProcessor.getSynthEngine().setNoiseType(type);
  } else if (comboBox == &arpModeSelector) {
    auto mode = static_cast<Arpeggiator::Mode>(arpModeSelector.getSelectedId() - 1);
    audioProcessor.getSynthEngine().setArpeggiatorMode(mode);
  }
}

void DrawFormAudioProcessorEditor::updateEnvelope() {
  audioProcessor.getSynthEngine().setEnvelopeParameters(
    static_cast<float>(attackSlider.getValue()),
    static_cast<float>(decaySlider.getValue()),
    static_cast<float>(sustainSlider.getValue()),
    static_cast<float>(releaseSlider.getValue())
  );
}
