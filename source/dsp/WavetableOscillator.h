#pragma once
#include "Wavetable.h"

class WavetableOscillator {
public:
  WavetableOscillator() {
    randomizeDriftParameters();
  }

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

    // analog drift
    float driftOffset = processDrift();

    phase += phaseIncrement * (1.0f + driftOffset);
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }

    return sample * gain;
  }

  void reset() {
    phase = 0.0f;
    playing = false;
    randomizeDriftParameters();
  }

private:
  void randomizeDriftParameters() {
    // randomize slow drift parameters
    slowDriftRate = 0.1f + random.nextFloat() * 0.4f;  // 0.1 - 0.5 hz
    slowDriftState = random.nextFloat() * 2.0f - 1.0f;
    slowDriftTarget = random.nextFloat() * 2.0f - 1.0f;
    slowDriftSmoothing = 0.99f + random.nextFloat() * 0.009f;

    // randomize fast drift parameters
    fastDriftRate = 5.0f + random.nextFloat() * 10.0f;  // 5 - 15 hz
    fastDriftState = random.nextFloat() * 2.0f - 1.0f;
    fastDriftTarget = random.nextFloat() * 2.0f - 1.0f;
    fastDriftSmoothing = 0.95f + random.nextFloat() * 0.04f;
  }

  float processDrift() {
    if (driftAmount == 0.0f) {
      return 0.0f;
    }

    float dt = 1.0f / static_cast<float>(sampleRate);

    // slow drift (thermal drift)
    slowDriftTimer += dt;
    if (slowDriftTimer >= 1.0f / slowDriftRate) {
      slowDriftTimer = 0.0f;
      slowDriftTarget = random.nextFloat() * 2.0f - 1.0f;
    }
    slowDriftState = slowDriftState * slowDriftSmoothing + slowDriftTarget * (1.0f - slowDriftSmoothing);

    // fast drift (component noise)
    fastDriftTimer += dt;
    if (fastDriftTimer >= 1.0f / fastDriftRate) {
      fastDriftTimer = 0.0f;
      fastDriftTarget = random.nextFloat() * 2.0f - 1.0f;
    }
    fastDriftState = fastDriftState * fastDriftSmoothing + fastDriftTarget * (1.0f - fastDriftSmoothing);

    // combine layers
    float slowComponent = slowDriftState * driftAmount * 0.009f;   // ~15 cents max
    float fastComponent = fastDriftState * driftAmount * 0.0018f;  // ~3 cents max

    return slowComponent + fastComponent;
  }

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

  // drift amount
  float driftAmount = 0.0f;

  float slowDriftRate = 0.3f;
  float slowDriftState = 0.0f;
  float slowDriftTarget = 0.0f;
  float slowDriftTimer = 0.0f;
  float slowDriftSmoothing = 0.995f;

  float fastDriftRate = 10.0f;
  float fastDriftState = 0.0f;
  float fastDriftTarget = 0.0f;
  float fastDriftTimer = 0.0f;
  float fastDriftSmoothing = 0.97f;

  juce::Random random;
};
