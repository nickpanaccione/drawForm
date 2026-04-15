#pragma once

#include <JuceHeader.h>
#include "WavetableFrame.h"
#include <array>
#include <cmath>
#include <vector>

class Wavetable {
public:
  static constexpr int kTableSize = WavetableFrame::kTableSize;
  static constexpr int kDefaultNumFrames = 4;

  Wavetable(int numFrames = kDefaultNumFrames) {
    frames.resize(numFrames);
    initializeDefaultFrames();
  }

  void initializeDefaultFrames() {
    if (frames.size() >= 1) { frames[0].fillSine(); }
    if (frames.size() >= 2) { frames[1].fillTriangle(); }
    if (frames.size() >= 3) { frames[2].fillSaw(); }
    if (frames.size() >= 4) { frames[3].fillSquare(); }
  }

  int getNumFrames() const { return static_cast<int>(frames.size()); }

  WavetableFrame& getFrame(int index) {
    return frames[juce::jlimit(0, static_cast<int>(frames.size()) - 1, index)];
  }

  const WavetableFrame& getFrame(int index) const {
    return frames[juce::jlimit(0, static_cast<int>(frames.size()) - 1, index)];
  }

  void setSample(int frameIndex, int sampleIndex, float value) {
    if (frameIndex >= 0 && frameIndex < static_cast<int>(frames.size())) {
      frames[frameIndex].setSample(sampleIndex, value);
    }
  }

  float getInterpolatedSample(float tableIndex, float framePosition) const {
    int numFrames = static_cast<int>(frames.size());
    float scaledPosition = framePosition * (numFrames - 1);

    int frame0 = static_cast<int>(scaledPosition);
    int frame1 = frame0 + 1;

    frame0 = juce::jlimit(0, numFrames - 1, frame0);
    frame1 = juce::jlimit(0, numFrames - 1, frame1);

    float frameFrac = scaledPosition - static_cast<float>(static_cast<int>(scaledPosition));

    float sample0 = frames[frame0].getInterpolatedSample(tableIndex);
    float sample1 = frames[frame1].getInterpolatedSample(tableIndex);

    return sample0 + frameFrac * (sample1 - sample0);
  }

  // legacy compatibility
  void setSample(int index, float value) { setSample(0, index, value); }
  float getSample(int index) const { return frames[0].getSample(index); }
  float getInterpolatedSample(float index) const { return frames[0].getInterpolatedSample(index); }
  const std::array<float, kTableSize>& getTable() const { return frames[0].getTable(); }
  std::array<float, kTableSize>& getTable() { return frames[0].getTable(); }

private:
  std::vector<WavetableFrame> frames;
};
