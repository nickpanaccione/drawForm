#pragma once
#include <JuceHeader.h>

class NoiseOscillator {
public:
  enum class NoiseType {
    White,
    Pink,
    Brown
  };

  void setNoiseType(NoiseType type) {
    noiseType = type;
  }

  void setLevel(float level) {
    noiseLevel = juce::jlimit(0.0f, 1.0f, level);
  }

  float process() {
    float noise = 0.0f;

    switch (noiseType) {
    case NoiseType::White:
      noise = processWhite();
      break;
    case NoiseType::Pink:
      noise = processPink();
      break;
    case NoiseType::Brown:
      noise = processBrown();
      break;
    }

    return noise * noiseLevel;
  }

  void reset() {
    pinkB0 = 0.0f;
    pinkB1 = 0.0f;
    pinkB2 = 0.0f;
    pinkB3 = 0.0f;
    pinkB4 = 0.0f;
    pinkB5 = 0.0f;
    pinkB6 = 0.0f;
    brownState = 0.0f;
  }

private:
  float processWhite() {
    return random.nextFloat() * 2.0f - 1.0f;
  }

  float processPink() {
    float white = processWhite();

    pinkB0 = 0.99886f * pinkB0 + white * 0.0555179f;
    pinkB1 = 0.99332f * pinkB1 + white * 0.0750759f;
    pinkB2 = 0.96900f * pinkB2 + white * 0.1538520f;
    pinkB3 = 0.86650f * pinkB3 + white * 0.3104856f;
    pinkB4 = 0.55000f * pinkB4 + white * 0.5329522f;
    pinkB5 = -0.7616f * pinkB5 - white * 0.0168980f;

    float pink = pinkB0 + pinkB1 + pinkB2 + pinkB3 + pinkB4 + pinkB5 + pinkB6 + white * 0.5362f;
    pinkB6 = white * 0.115926f;

    return pink * 0.11f;
  }

  float processBrown() {
    float white = processWhite();
    brownState = (brownState + (0.02f * white)) / 1.02f;
    return brownState * 3.5f;
  }

  juce::Random random;
  NoiseType noiseType = NoiseType::White;
  float noiseLevel = 0.0f;

  // pink noise
  float pinkB0 = 0.0f;
  float pinkB1 = 0.0f;
  float pinkB2 = 0.0f;
  float pinkB3 = 0.0f;
  float pinkB4 = 0.0f;
  float pinkB5 = 0.0f;
  float pinkB6 = 0.0f;

  // brown noise
  float brownState = 0.0f;
};
