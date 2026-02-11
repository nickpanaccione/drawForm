#pragma once

#include <JuceHeader.h>
#include <array>
#include <cmath>

class Wavetable {
public:
  static constexpr int kTableSize = 2048;

  Wavetable() {
    fillSine();
  }

  void fillSine() {
    for (int i = 0; i < kTableSize; ++i) {
      float phase = static_cast<float>(i) / static_cast<float>(kTableSize);
      table[i] = std::sin(2.0f * juce::MathConstants<float>::pi * phase);
    }
  }

  void fillSaw() {
    for (int i = 0; i < kTableSize; ++i) {
      float phase = static_cast<float>(i) / static_cast<float>(kTableSize);
      table[i] = 2.0f * phase - 1.0f;
    }
  }

  void fillSquare() {
    for (int i = 0; i < kTableSize; ++i) {
      table[i] = (i < kTableSize / 2) ? 1.0f : -1.0f;
    }
  }

  void setSample(int index, float value) {
    if (index >= 0 && index < kTableSize) {
      table[index] = juce::jlimit(-1.0f, 1.0f, value);
    }
  }

  float getSample(int index) const {
    return table[index & (kTableSize - 1)];
  }

  float getInterpolatedSample(float index) const {
    int i0 = static_cast<int>(index);
    int i1 = i0 + 1;
    float frac = index - static_cast<float>(i0);

    float s0 = getSample(i0);
    float s1 = getSample(i1);

    return s0 + frac * (s1 - s0); 
  }

  const std::array<float, kTableSize>& getTable() const { return table; }
  std::array<float, kTableSize>& getTable() { return table; }

private:
  std::array<float, kTableSize> table{};
};
