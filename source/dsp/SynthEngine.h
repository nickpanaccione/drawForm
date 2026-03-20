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

  void setEnvelopeParameters(float attack, float decay, float sustain, float release) {
    for (auto& voice : voices) {
      voice.setEnvelopeParameters(attack, decay, sustain, release);
    }
  }

  void noteOn(int note, float velocity) {
    // check if not is playing
    for (auto& voice : voices) {
      if (voice.getCurrentNote() == note) {
        voice.noteOn(note, velocity);
        return;
      }
    }

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


  void setFramePosition(float position) {
    for (auto& voice : voices) {
      voice.setFramePosition(position);
    }
  }

  void setDriftAmount(float amount) {
    for (auto& voice : voices) {
      voice.setDriftAmount(amount);
    }
  }

  void setNoiseLevel(float level) {
    for (auto& voice : voices) {
      voice.setNoiseLevel(level);
    }
  }

  void setNoiseType(NoiseOscillator::NoiseType type) {
    for (auto& voice : voices) {
      voice.setNoiseType(type);
    }
  }
  void setPitchBend(float normalizedValue) {
    float semitones = normalizedValue * pitchBendRange;
    for (auto& voice : voices) {
      voice.setPitchBend(semitones);
    }
  }

  void setPitchBendRange(float semitones) {
    pitchBendRange = semitones;
    for (auto& voice : voices) {
      voice.setPitchBendRange(semitones);
    }
  }

  void setDetune(float cents) {
    for (auto& voice : voices) {
      voice.setDetune(cents);
    }
  }

private:
  Wavetable wavetable;
  std::array<SynthVoice, kMaxVoices> voices;
  float pitchBendRange = 2.0f;
};
