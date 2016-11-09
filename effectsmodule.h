#ifndef EFFECTS_MODULE_H_
#define EFFECTS_MODULE_H_

#include "module.h"
#include "graphics.h"
#include "metronome.h"

class EffectsModule : public Module
{
 public:
  EffectsModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
  bool update(MetronomeState state);

 private:
  Graphics *gfx;
};

#endif
