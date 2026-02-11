#pragma once

#include "SynthVoice.h"
#include <array>

class SynthEngine {
public:
  static constexpr int kMaxVoices = 8;

  SynthEngine() {
    for (auto& voice : voices) {
      voice.setWavetable(&wavetable);
    }
  }

  void setSampleRate(double sampleRate) {
    for (auto& voice : voices) {
    voice.setSampleRate(sampleRate);
    }
  }

  void noteOn(int note, float velocity) {
  // find free voice 
    for (auto& voice : voices) {
      if (!voice.isPlaying()) {
        voice.noteOn(note, velocity);
        return;
      }
    }
    // voices busy steal first 
    voices[0].noteOn(note, velocity);
  }

  void noteOff(int note) {
    for (auto& voice : voices) {
      voice.noteOff(note);
    }
  }

  float process() {
    float output = 0.0f;
    for (auto& voice : voices) {
      if (voice.isPlaying()) {
          output += voice.process();
      }
    }
    return output * 0.25f; // gain staging
  }

  Wavetable& getWavetable() { return wavetable; }

private:
  Wavetable wavetable;
  std::array<SynthVoice, kMaxVoices> voices;
};
