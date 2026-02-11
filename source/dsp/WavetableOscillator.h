#pragma once

#include "Wavetable.h"

class WavetableOscillator {
public:
  void setWavetable(Wavetable* wt) {
    wavetable = wt;
  }

  void setSampleRate(double sr) {
    sampleRate = sr;
    updatePhaseIncrement();
  }

  void setFrequency(float freq) {
    frequency = freq;
    updatePhaseIncrement();
  }

  void noteOn(int midiNote, float velocity) {
    frequency = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    gain = velocity;
    playing = true;
    updatePhaseIncrement();
  }

  void noteOff() {
  playing = false;
  }

  bool isPlaying() const { return playing; }

  float process() {
    if (!playing || wavetable == nullptr) {
      return 0.0f;
    }

    float tableIndex = phase * static_cast<float>(Wavetable::kTableSize);
    float sample = wavetable->getInterpolatedSample(tableIndex);

    phase += phaseIncrement;
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }

    return sample * gain;
  }

  void reset() {
    phase = 0.0f;
    playing = false;
  }

private:
  void updatePhaseIncrement() {
    if (sampleRate > 0.0) {
      phaseIncrement = frequency / static_cast<float>(sampleRate);
    }
  }

  Wavetable* wavetable = nullptr;
  double sampleRate = 44100.0;
  float frequency = 440.0f;
  float phase = 0.0f;
  float phaseIncrement = 0.0f;
  float gain = 1.0f;
  bool playing = false;
};
