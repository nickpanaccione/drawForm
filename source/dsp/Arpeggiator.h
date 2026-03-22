#pragma once

#include <JuceHeader.h>
#include <vector>
#include <algorithm>

class Arpeggiator {
public:
  enum class Mode {
    Up,
    Down,
    UpDown,
    Random
  };

  void setSampleRate(double sr) {
    sampleRate = sr;
  }

  void setEnabled(bool shouldBeEnabled) {
    enabled = shouldBeEnabled;
    if (!enabled) {
      reset();
    }
  }

  bool isEnabled() const { return enabled; }

  void setRate(float beatsPerSecond) {
    rate = beatsPerSecond;
    samplesPerStep = static_cast<int>(sampleRate / rate);
  }

  void setMode(Mode newMode) {
    mode = newMode;
    currentStep = 0;
    goingUp = true;
  }

  void noteOn(int note) {
    if (std::find(heldNotes.begin(), heldNotes.end(), note) == heldNotes.end()) {
      heldNotes.push_back(note);
      sortNotes();
    }
  }

  void noteOff(int note) {
    heldNotes.erase(std::remove(heldNotes.begin(), heldNotes.end(), note), heldNotes.end());

    if (heldNotes.empty()) {
      shouldReleaseCurrentNote = true;
    }
  }

  void process(int numSamples, std::function<void(int, float)> triggerNote, std::function<void(int)> releaseNote) {
    if (!enabled) {
      return;
    }

    if (shouldReleaseCurrentNote && currentNote >= 0) {
      releaseNote(currentNote);
      currentNote = -1;
      shouldReleaseCurrentNote = false;
      sampleCounter = 0;
      return;
    }

    if (heldNotes.empty()) {
      return;
    }

    for (int i = 0; i < numSamples; ++i) {
      sampleCounter++;

      if (sampleCounter >= samplesPerStep) {
        sampleCounter = 0;

        if (currentNote >= 0) {
          releaseNote(currentNote);
        }

        currentNote = getNextNote();

        if (currentNote >= 0) {
          triggerNote(currentNote, 0.8f);
        }
      }
    }
  }

  void reset() {
    currentStep = 0;
    currentNote = -1;
    sampleCounter = 0;
    goingUp = true;
    shouldReleaseCurrentNote = false;
    heldNotes.clear();
  }

  int getCurrentNote() const { return currentNote; }

private:
  void sortNotes() {
    std::sort(heldNotes.begin(), heldNotes.end());
  }

  int getNextNote() {
    if (heldNotes.empty()) {
      return -1;
    }

    int note = -1;

    switch (mode) {
      case Mode::Up:
        note = heldNotes[currentStep % heldNotes.size()];
        currentStep++;
        break;

      case Mode::Down:
        note = heldNotes[(heldNotes.size() - 1) - (currentStep % heldNotes.size())];
        currentStep++;
        break;

      case Mode::UpDown:
        if (heldNotes.size() == 1) {
          note = heldNotes[0];
        } else {
          note = heldNotes[currentStep];

          if (goingUp) {
            currentStep++;
            if (currentStep >= static_cast<int>(heldNotes.size()) - 1) {
              goingUp = false;
            }
          } else {
            currentStep--;
            if (currentStep <= 0) {
              goingUp = true;
            }
          }
        }
        break;

      case Mode::Random:
        note = heldNotes[random.nextInt(static_cast<int>(heldNotes.size()))];
        break;
    }

    return note;
  }

  double sampleRate = 44100.0;
  bool enabled = false;
  float rate = 4.0f;
  int samplesPerStep = 11025;
  int sampleCounter = 0;

  Mode mode = Mode::Up;
  std::vector<int> heldNotes;
  int currentStep = 0;
  int currentNote = -1;
  bool goingUp = true;
  bool shouldReleaseCurrentNote = false;

  juce::Random random;
};
