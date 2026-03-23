#include "PluginProcessor.h"
#include "PluginEditor.h"

DrawFormAudioProcessorEditor::DrawFormAudioProcessorEditor(DrawFormAudioProcessor& p)
  : AudioProcessorEditor(&p),
    audioProcessor(p),
    wavetableEditor(p.getSynthEngine().getWavetable()) {
  addAndMakeVisible(wavetableEditor);

  auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double initial) {
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

    updateEnvelope();

    setSize(700, 450);
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

  bounds.removeFromTop(10);

  auto waveformArea = bounds.removeFromTop(200);
  wavetableEditor.setBounds(waveformArea);

  bounds.removeFromTop(20);

  auto sliderWidth = bounds.getWidth() / 5;
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
}

void DrawFormAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
  if (slider == &morphSlider) {
    audioProcessor.getSynthEngine().setFramePosition(static_cast<float>(morphSlider.getValue()));
  } else {
    updateEnvelope();
  }
}

void DrawFormAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox) {
  if (comboBox == &frameSelector) {
    wavetableEditor.setCurrentFrame(frameSelector.getSelectedId() - 1);
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
