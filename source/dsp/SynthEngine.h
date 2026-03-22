#pragma once

#include "SynthVoice.h"
#include "Arpeggiator.h"
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
    this->sampleRate = sampleRate;
    arpeggiator.setSampleRate(sampleRate);

    for (auto& voice : voices) {
      voice.setSampleRate(sampleRate);
    }
  }

  void setEnvelopeParameters(float attack, float decay, float sustain, float release) {
    for (auto& voice : voices) {
      voice.setEnvelopeParameters(attack, decay, sustain, release);
    }
  }

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

  void setArpeggiatorEnabled(bool enabled) {
    arpeggiator.setEnabled(enabled);
  }

  void setArpeggiatorRate(float rate) {
    arpeggiator.setRate(rate);
  }

  void setArpeggiatorMode(Arpeggiator::Mode mode) {
    arpeggiator.setMode(mode);
  }

  void noteOn(int note, float velocity) {
    if (arpeggiator.isEnabled()) {
        arpeggiator.noteOn(note);
    } else {
      triggerNote(note, velocity);
    }
  }

  void noteOff(int note) {
    if (arpeggiator.isEnabled()) {
      arpeggiator.noteOff(note);
    } else {
      releaseNote(note);
    }
  }

  float process() {
    arpeggiator.process(1,
      [this](int note, float velocity) { triggerNote(note, velocity); },
      [this](int note) { releaseNote(note); }
    );

    float output = 0.0f;
    for (auto& voice : voices) {
      if (voice.isPlaying()) {
        output += voice.process();
      }
    }
    return output * 0.25f;
  }

  Wavetable& getWavetable() { return wavetable; }

private:
  void triggerNote(int note, float velocity) {
    for (auto& voice : voices) {
      if (voice.getCurrentNote() == note) {
        voice.noteOn(note, velocity);
        return;
      }
    }

    for (auto& voice : voices) {
      if (!voice.isPlaying()) {
        voice.noteOn(note, velocity);
        return;
      }
    }

    voices[0].noteOn(note, velocity);
  }

  void releaseNote(int note) {
    for (auto& voice : voices) {
      voice.noteOff(note);
    }
  }

  Wavetable wavetable;
  std::array<SynthVoice, kMaxVoices> voices;
  Arpeggiator arpeggiator;
  float pitchBendRange = 2.0f;
  double sampleRate = 44100.0;
};
