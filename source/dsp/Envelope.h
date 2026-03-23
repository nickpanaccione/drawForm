#pragma once

#include <JuceHeader.h>

class Envelope {
public:
  enum class State {
    Idle,
    Attack,
    Decay,
    Sustain,
    Release
  };

  void setSampleRate(double sr) {
    sampleRate = sr;
    recalculateRates();
  }

  void setAttack(float seconds) {
    attackTime = seconds;
    recalculateRates();
  }

  void setDecay(float seconds) {
    decayTime = seconds;
    recalculateRates();
  }

  void setSustain(float level) {
    sustainLevel = juce::jlimit(0.0f, 1.0f, level);
  }

  void setRelease(float seconds) {
    releaseTime = seconds;
    recalculateRates();
  }

  void setParameters(float attack, float decay, float sustain, float release) {
    attackTime = attack;
    decayTime = decay;
    sustainLevel = juce::jlimit(0.0f, 1.0f, sustain);
    releaseTime = release;
    recalculateRates();
  }

  void noteOn() {
    state = State::Attack;
  }

  void noteOff() {
    if (state != State::Idle) {
      state = State::Release;
    }
  }

  bool isActive() const {
    return state != State::Idle;
  }

  float process() {
    switch (state) {
      case State::Idle:
        return 0.0f;

      case State::Attack:
        currentLevel += attackRate;
        if (currentLevel >= 1.0f) {
            currentLevel = 1.0f;
            state = State::Decay;
        }
        break;

      case State::Decay:
        currentLevel -= decayRate;
        if (currentLevel <= sustainLevel) {
          currentLevel = sustainLevel;
          state = State::Sustain;
        }
        break;

      case State::Sustain:
        currentLevel = sustainLevel;
        break;

      case State::Release:
        currentLevel -= releaseRate;
        if (currentLevel <= 0.0f) {
          currentLevel = 0.0f;
          state = State::Idle;
        }
        break;
    }

    return currentLevel;
  }

  void reset() {
    state = State::Idle;
    currentLevel = 0.0f;
  }

  State getState() const { return state; }

private:
  void recalculateRates() {
    if (sampleRate <= 0.0) {
      return;
    }

    attackRate = attackTime > 0.0f ? 1.0f / (attackTime * static_cast<float>(sampleRate)) : 1.0f;
    decayRate = decayTime > 0.0f ? (1.0f - sustainLevel) / (decayTime * static_cast<float>(sampleRate)) : 1.0f;
    releaseRate = releaseTime > 0.0f ? sustainLevel / (releaseTime * static_cast<float>(sampleRate)) : 1.0f;
  }

  double sampleRate = 44100.0;
  State state = State::Idle;
  float currentLevel = 0.0f;

  float attackTime = 0.01f;
  float decayTime = 0.1f;
  float sustainLevel = 0.7f;
  float releaseTime = 0.3f;

  float attackRate = 0.0f;
  float decayRate = 0.0f;
  float releaseRate = 0.0f;
};
