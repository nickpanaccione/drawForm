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

  void setFramePosition(float position) {
    framePosition = juce::jlimit(0.0f, 1.0f, position);
  }

  void setDriftAmount(float amount) {
    driftAmount = juce::jlimit(0.0f, 1.0f, amount);
  }

  void setPitchBend(float semitones) {
    pitchBendSemitones = semitones;
    updatePhaseIncrement();
  }

  void setPitchBendRange(float semitones) {
    pitchBendRange = semitones;
  }

  void setDetune(float cents) {
    detuneCents = cents;
    updatePhaseIncrement();
  }

  void noteOn(int midiNote, float velocity) {
    baseFrequency = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
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
    float sample = wavetable->getInterpolatedSample(tableIndex, framePosition);

    // update drift LFO
    driftPhase += driftSpeed / static_cast<float>(sampleRate);
    if (driftPhase >= 1.0f) {
      driftPhase -= 1.0f;
    }

    // drift offset
    float driftLFO = std::sin(2.0f * juce::MathConstants<float>::pi * driftPhase);
    float driftOffset = driftLFO * driftAmount * 0.01f; // Max ~1% pitch drift

    phase += phaseIncrement * (1.0f + driftOffset);
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }

    return sample * gain;
  }

  void reset() {
    phase = 0.0f;
    driftPhase = random.nextFloat(); // randomize phase per voice
    playing = false;
  }

private:
  void updatePhaseIncrement() {
    if (sampleRate > 0.0) {
      float pitchOffset = pitchBendSemitones + (detuneCents / 100.0f);
      frequency = baseFrequency * std::pow(2.0f, pitchOffset / 12.0f);
      phaseIncrement = frequency / static_cast<float>(sampleRate);
    }
  }

  Wavetable* wavetable = nullptr;
  double sampleRate = 44100.0;
  float baseFrequency = 440.0f;
  float frequency = 440.0f;
  float phase = 0.0f;
  float phaseIncrement = 0.0f;
  float gain = 1.0f;
  float framePosition = 0.0f;
  bool playing = false;

  // pitch bend and detune
  float pitchBendSemitones = 0.0f;
  float pitchBendRange = 2.0f;
  float detuneCents = 0.0f;

  // drift parameters
  float driftAmount = 0.0f;
  float driftPhase = 0.0f;
  float driftSpeed = 0.3f; 

  juce::Random random;
};
