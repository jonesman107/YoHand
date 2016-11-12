#ifndef SYNTHMODULE_H
#define SYNTHMODULE_H

#include "instrumentmodule.h"
#include "graphics.h"
#include "metronome.h"


class SynthModule : public InstrumentModule
{
 public:
  SynthModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
  void update(MetronomeState state);

 private:
  Graphics *gfx;
  int curSubdiv;
  int sequence[16];
  int curPos;
  int melodyNote;
  int chordChange[4];

  void stopAllNotes();
  void sendMelody(MetronomeState state);
  void updateNotesIfNeeded(MetronomeState state);
  void sendEnvelope(int attack, int decay, float sustain, int release);
  void checkInstrumentChange();
};

#endif // SYNTHMODULE_H
