#ifndef METRONOME_H_
#define METRONOME_H_

#include <QTimer>
#include <iostream>

#define MAX_SUBDIV (4 * 4 * 3)
#define QUARTER_SUBDIV (4 * 3)
#define EIGHTH_SUBDIV (2 * 3)
#define SIXTEENTH_SUBDIV (3)
#define TRIPLET_SUBDIV (4 * 4)
#define EIGHTH_TRIPLET_SUBDIV (4)

struct MetronomeState
{
  int measuresElapsed;

  bool measure;
  bool quarter;
  bool triplet;
  bool eighth;
  bool sixteenth;
  bool eighthTriplet;
  int quarterNum;
  int tripletNum;
  int eighthNum;
  int sixteenthNum;
  int eighthTripletNum;
  int sixteenthTripletNum;
};

class Metronome
{
 private:
  int bpm;
  int count;
  QTimer *t;

 public:
 Metronome(QTimer *t, int bpm) : t(t) {
    setBPM(bpm);
  }

  QTimer *timer() { return t; }
  void setBPM(int bpm)
  {
    this->bpm = bpm;
    t->setInterval(60 * 1000 * 4 / (bpm * MAX_SUBDIV));
    std::cout << "BPM: " << bpm << std::endl;
  }

  int getBPM()
  {
    return bpm;
  }

  void tick()
  {
    count++;
  }

  MetronomeState state()
  {
    MetronomeState state;
    state.measure = !(count % MAX_SUBDIV);
    state.quarter = !(count % QUARTER_SUBDIV);
    state.eighth = !(count % EIGHTH_SUBDIV);
    state.triplet = !(count % TRIPLET_SUBDIV);
    state.sixteenth = !(count % SIXTEENTH_SUBDIV);
    state.eighthTriplet = !(count % EIGHTH_TRIPLET_SUBDIV);
    state.measuresElapsed = count / MAX_SUBDIV;
    int countInBeat = count - state.measuresElapsed * MAX_SUBDIV;
    state.quarterNum = countInBeat / QUARTER_SUBDIV;
    state.eighthNum = countInBeat / EIGHTH_SUBDIV;
    state.tripletNum = countInBeat / TRIPLET_SUBDIV;
    state.sixteenthNum = countInBeat / SIXTEENTH_SUBDIV;
    state.eighthTripletNum = countInBeat / EIGHTH_TRIPLET_SUBDIV;
    state.sixteenthTripletNum = countInBeat;
    return state;
  }
};

#endif
