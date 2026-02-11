#pragma once

#include "WavetableOscillator.h"

class SynthVoice {
public:
  void setWavetable(Wavetable* wt) {
    oscillator.setWavetable(wt);
  }

  void setSampleRate(double sampleRate) {
    oscillator.setSampleRate(sampleRate);
  }

  void noteOn(int note, float velocity) {
    currentNote = note;
    oscillator.noteOn(note, velocity);
  }

  void noteOff(int note) {
    if (note == currentNote) {
      oscillator.noteOff();
      currentNote = -1;
    }
  }

  bool isPlaying() const { return oscillator.isPlaying(); }
  int getCurrentNote() const { return currentNote; }

  float process() {
    return oscillator.process();
  }

private:
  WavetableOscillator oscillator;
  int currentNote = -1;
};
