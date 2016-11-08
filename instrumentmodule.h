#ifndef INSTRUMENT_MODULE_H_
#define INSTRUMENT_MODULE_H_

#include "module.h"
#include "graphics.h"
#include "metronome.h"

//// miliseconds in one beat
//static int BEAT_PERIOD = 200;

#define BEAT_QUARTER   0
#define BEAT_EIGHTH    1
#define BEAT_SIXTEENTH 2
#define BEAT_SIXTRIPLE 2

struct BeatState
{
  bool beat;
  int measureChanged;
  int type;
};

class Rhythm
{
 private:
  BeatState accents[MAX_SUBDIV];
 public:
  Rhythm();
  void incrAccent(MetronomeState state);
  void clearAccents();
  bool accentAt(MetronomeState state);
  int numAccents();
  void getAccents(bool *arr);

};

class InstrumentModule : public Module
{
 public:
  InstrumentModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
  void update(MetronomeState state);
  void changeInstrument(int instrumentNumber);
  void sendEnvelope(int attack, int decay, float sustain, int release);
  void sendCompressor(int instrumentNumber);
  void incrementKey();
  void decrementKey();

 private:
  Graphics *gfx;
  int changeKey;
  int prevRoot;


  int curSubdiv;
  int sequence[16];
  int curPos;
  int melodyNote;

  int prevTriad[3];
  int chordChange[4];
  int chordChannel;
  int prevInstrument;
  int currentInstrument;
  std::vector<int> offQueue;

  // Rhythm params.
  Rhythm rhythm;

  void checkInstrumentChange();
  bool checkStop();
  void stopAllNotes();
  void updateRhythm(MetronomeState state);
  void stopTriadIfNeeded(int *triad);
  void updateCurrentSubdivision();
  void sendNote(int channel, int note, int velocity, int *updateNote);
  void sendMelody(MetronomeState state);
  void updateNotesIfNeeded(MetronomeState state);
  void sendChordsIfNeeded(MetronomeState state);
};

#endif
