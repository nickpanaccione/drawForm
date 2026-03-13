#pragma once

#include "WavetableOscillator.h"
#include "Envelope.h"

class SynthVoice {
public:
  void setWavetable(Wavetable* wt) {
    oscillator.setWavetable(wt);
  }

  void setSampleRate(double sampleRate) {
    oscillator.setSampleRate(sampleRate);
    envelope.setSampleRate(sampleRate);
  }

  void setEnvelopeParameters(float attack, float decay, float sustain, float release) {
    envelope.setParameters(attack, decay, sustain, release);
  }

  void noteOn(int note, float velocity) {
    currentNote = note;
    velocityGain = velocity;
    oscillator.noteOn(note, 1.0f);
    envelope.noteOn();
  }

  void noteOff(int note) {
    if (note == currentNote) {
      envelope.noteOff();
    }
  }

  bool isPlaying() const { return oscillator.isPlaying(); }
  int getCurrentNote() const { return currentNote; }

  float process() {
    if (!envelope.isActive()) {
      if (oscillator.isPlaying()) {
        oscillator.noteOff();
        currentNote = -1;
      }
      return 0.0f;
    }

    float envLevel = envelope.process();
    float sample = oscillator.process();
    return sample * envLevel * velocityGain;
  }

private:
  WavetableOscillator oscillator;
  Envelope envelope;
  int currentNote = -1;
  float velocityGain = 1.0f;
};
