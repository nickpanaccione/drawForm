#pragma once

#include <JuceHeader.h>
#include "../dsp/Wavetable.h"

class WavetableEditor : public juce::Component {
public:
  WavetableEditor(Wavetable& wt) : wavetable(wt) {
    setRepaintsOnMouseActivity(true);
  }

  void setCurrentFrame(int frame) {
    currentFrame = juce::jlimit(0, wavetable.getNumFrames() - 1, frame);
    repaint();
  }

  int getCurrentFrame() const { return currentFrame; }

  void setMorphPosition(float position) {
    morphPosition = juce::jlimit(0.0f, 1.0f, position);
    repaint();
  }

  void paint(juce::Graphics& g) override {
    auto bounds = getLocalBounds().toFloat();

    g.fillAll(juce::Colours::black);

    // draw grid
    g.setColour(juce::Colours::darkgrey);
    g.drawHorizontalLine(static_cast<int>(bounds.getCentreY()), 0.0f, bounds.getWidth());

    // draw all frames 
    for (int f = 0; f < wavetable.getNumFrames(); ++f) {
      if (f == currentFrame) { 
        continue;
      }

      g.setColour(juce::Colours::grey.withAlpha(0.2f));
      drawFrame(g, bounds, f);
    }

    // draw current editable frame
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    drawFrame(g, bounds, currentFrame);

    // draw morphed waveform 
    g.setColour(juce::Colours::cyan);
    drawMorphedWaveform(g, bounds);

    // draw morph position indicator
    drawMorphIndicator(g, bounds);
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
  void drawFrame(juce::Graphics& g, juce::Rectangle<float> bounds, int frameIndex) {
    juce::Path wavePath;
    const auto& frame = wavetable.getFrame(frameIndex);

    for (int i = 0; i < Wavetable::kTableSize; ++i) {
      float x = (static_cast<float>(i) / static_cast<float>(Wavetable::kTableSize - 1)) * bounds.getWidth();
      float y = bounds.getCentreY() - (frame.getTable()[i] * bounds.getHeight() * 0.45f);

      if (i == 0) {
        wavePath.startNewSubPath(x, y);
      } else { 
        wavePath.lineTo(x, y);
      }
    }

    g.strokePath(wavePath, juce::PathStrokeType(frameIndex == currentFrame ? 2.0f : 1.0f));
  }

  void drawMorphedWaveform(juce::Graphics& g, juce::Rectangle<float> bounds) {
    juce::Path wavePath;
    constexpr int step = 4;
    for (int i = 0; i < Wavetable::kTableSize; i += step) {
      float x = (static_cast<float>(i) / static_cast<float>(Wavetable::kTableSize - 1)) * bounds.getWidth();
      float sample = wavetable.getInterpolatedSample(static_cast<float>(i), morphPosition);
      float y = bounds.getCentreY() - (sample * bounds.getHeight() * 0.45f);

      if (i == 0) {
        wavePath.startNewSubPath(x, y);
      } else {
        wavePath.lineTo(x, y);
      }
    }

    g.strokePath(wavePath, juce::PathStrokeType(2.5f));
  }

  void drawMorphIndicator(juce::Graphics& g, juce::Rectangle<float> bounds) {
    int numFrames = wavetable.getNumFrames();
    float indicatorWidth = bounds.getWidth();
    float indicatorHeight = 8.0f;
    float indicatorY = bounds.getBottom() - indicatorHeight - 5.0f;

    // draw frame markers
    g.setColour(juce::Colours::grey);
    for (int i = 0; i < numFrames; ++i) {
      float x = (static_cast<float>(i) / static_cast<float>(numFrames - 1)) * indicatorWidth;
      g.fillEllipse(x - 3.0f, indicatorY, 6.0f, 6.0f);
    }

    // draw current morph position
    g.setColour(juce::Colours::cyan);
    float morphX = morphPosition * indicatorWidth;
    g.fillEllipse(morphX - 5.0f, indicatorY - 1.0f, 10.0f, 10.0f);

    // draw line connecting frames
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    g.drawLine(0.0f, indicatorY + 3.0f, indicatorWidth, indicatorY + 3.0f, 2.0f);
  }

  void drawLine(juce::Point<float> from, juce::Point<float> to) {
    auto bounds = getLocalBounds().toFloat();

    int fromIndex = static_cast<int>((from.x / bounds.getWidth()) * Wavetable::kTableSize);
    int toIndex = static_cast<int>((to.x / bounds.getWidth()) * Wavetable::kTableSize);

    fromIndex = juce::jlimit(0, Wavetable::kTableSize - 1, fromIndex);
    toIndex = juce::jlimit(0, Wavetable::kTableSize - 1, toIndex);

    float fromValue = 1.0f - (from.y / bounds.getHeight() * 2.0f);
    float toValue = 1.0f - (to.y / bounds.getHeight() * 2.0f);

    fromValue = juce::jlimit(-1.0f, 1.0f, fromValue);
    toValue = juce::jlimit(-1.0f, 1.0f, toValue);

    int step = (fromIndex <= toIndex) ? 1 : -1;
    int numSteps = std::abs(toIndex - fromIndex);

    if (numSteps == 0) {
      wavetable.setSample(currentFrame, toIndex, toValue);
    } else {
      for (int i = 0; i <= numSteps; ++i) {
        int index = fromIndex + (i * step);
        float t = static_cast<float>(i) / static_cast<float>(numSteps);
        float value = fromValue + t * (toValue - fromValue);
        wavetable.setSample(currentFrame, index, value);
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

    wavetable.setSample(currentFrame, index, sampleValue);
    repaint();
  }

  Wavetable& wavetable;
  juce::Point<float> lastPosition;
  int currentFrame = 0;
  float morphPosition = 0.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableEditor)
};
