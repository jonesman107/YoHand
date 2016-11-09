//#ifndef SYNTHMODULE_H
//#define SYNTHMODULE_H

//#include "module.h"
//#include "graphics.h"
//#include "metronome.h"


//class SynthModule : public InstrumentModule
//{
// public:
//  SynthModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
//  void update(MetronomeState state);
//  void changeInstrument(int instrumentNumber);
//  void sendEnvelope(int attack, int decay, float sustain, int release);
//  void sendCompressor(int instrumentNumber);
//  void incrementKey();
//  void decrementKey();

// private:
//  Graphics *gfx;
//  int changeKey;
//  int prevRoot;


//  int curSubdiv;
//  int sequence[16];
//  int curPos;
//  int melodyNote;

//  int prevTriad[3];
//  int chordChange[4];
//  int chordChannel;
//  int prevInstrument;
//  int currentInstrument;
//  std::vector<int> offQueue;

//  void checkInstrumentChange();
//  bool checkStop();
//  void stopAllNotes();
////  void updateRhythm(MetronomeState state);
//  void stopTriadIfNeeded(int *triad);
//  void updateCurrentSubdivision();
//  void sendNote(int channel, int note, int velocity, int *updateNote);
//  void sendMelody(MetronomeState state);
//  void updateNotesIfNeeded(MetronomeState state);
//  void sendChordsIfNeeded(MetronomeState state);
//};

//#endif // SYNTHMODULE_H
