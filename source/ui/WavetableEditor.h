#pragma once

#include <JuceHeader.h>
#include "../dsp/Wavetable.h"

class WavetableEditor : public juce::Component {
public:
  WavetableEditor(Wavetable& wt) : wavetable(wt) {
    setRepaintsOnMouseActivity(true);
  }

  void paint(juce::Graphics& g) override {
    auto bounds = getLocalBounds().toFloat();
  
    // background
    g.fillAll(juce::Colours::black);
  
    // draw grid lines
    g.setColour(juce::Colours::darkgrey);
    g.drawHorizontalLine(static_cast<int>(bounds.getCentreY()), 0.0f, bounds.getWidth());
  
    // draw waveform
    g.setColour(juce::Colours::cyan);
  
    juce::Path wavePath;
    const auto& table = wavetable.getTable();
  
    for (int i = 0; i < Wavetable::kTableSize; ++i) {
      float x = (static_cast<float>(i) / static_cast<float>(Wavetable::kTableSize - 1)) * bounds.getWidth();
      float y = bounds.getCentreY() - (table[i] * bounds.getHeight() * 0.45f);
      
      if (i == 0) {
        wavePath.startNewSubPath(x, y);
      } else {
        wavePath.lineTo(x, y);
      }
    }
  
    g.strokePath(wavePath, juce::PathStrokeType(2.0f));
  }

  void mouseDown(const juce::MouseEvent& event) override {
    lastPosition = event.position;
    drawAtPosition(event.position);
  }

  void mouseDrag(const juce::MouseEvent& event) override {
    drawLine(lastPosition, event.position);
    lastPosition = event.position;
  }

private:
  void drawLine(juce::Point<float> from, juce::Point<float> to) {
    auto bounds = getLocalBounds().toFloat();
    
    // convert pos to table indices
    int fromIndex = static_cast<int>((from.x / bounds.getWidth()) * Wavetable::kTableSize);
    int toIndex = static_cast<int>((to.x / bounds.getWidth()) * Wavetable::kTableSize);
    
    fromIndex = juce::jlimit(0, Wavetable::kTableSize - 1, fromIndex);
    toIndex = juce::jlimit(0, Wavetable::kTableSize - 1, toIndex);
    
    // convert y pos to sample values
    float fromValue = 1.0f - (from.y / bounds.getHeight() * 2.0f);
    float toValue = 1.0f - (to.y / bounds.getHeight() * 2.0f);
    
    fromValue = juce::jlimit(-1.0f, 1.0f, fromValue);
    toValue = juce::jlimit(-1.0f, 1.0f, toValue);
    
    int step = (fromIndex <= toIndex) ? 1 : -1;
    int numSteps = std::abs(toIndex - fromIndex);
    
    if (numSteps == 0) {
      wavetable.setSample(toIndex, toValue);
    } else {
      for (int i = 0; i <= numSteps; ++i) {
        int index = fromIndex + (i * step);
        float t = static_cast<float>(i) / static_cast<float>(numSteps);
        float value = fromValue + t * (toValue - fromValue);
        wavetable.setSample(index, value);
      }
    }
    
    repaint();
  }

  void drawAtPosition(juce::Point<float> pos) {
    auto bounds = getLocalBounds().toFloat();

    float normalizedX = pos.x / bounds.getWidth();
    int index = static_cast<int>(normalizedX * Wavetable::kTableSize);
    index = juce::jlimit(0, Wavetable::kTableSize - 1, index);

    float normalizedY = pos.y / bounds.getHeight();
    float sampleValue = 1.0f - (normalizedY * 2.0f);
    sampleValue = juce::jlimit(-1.0f, 1.0f, sampleValue);

    wavetable.setSample(index, sampleValue);
    repaint();
  }

  Wavetable& wavetable;
  juce::Point<float> lastPosition;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableEditor)
};
