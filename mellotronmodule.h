#ifndef MELLOTRON_MODULE_H_
#define MELLOTRON_MODULE_H_

#include "instrumentmodule.h"
#include "graphics.h"
#include "metronome.h"

#define BEAT_QUARTER   0
#define BEAT_EIGHTH    1
#define BEAT_SIXTEENTH 2
#define BEAT_SIXTRIPLE 2

class MellotronModule : public InstrumentModule
{
 public:
  MellotronModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
  void update(MetronomeState state);

 private:
  int prevRoot;
  int prevTriad[3];
  void sendChordsIfNeeded(MetronomeState state);
  void stopAllNotes();
  void checkInstrumentChange();
};

#endif
