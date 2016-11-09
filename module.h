#ifndef MODULE_H_
#define MODULE_H_

#include "leapmux.h"
#include "leappd.h"
#include "metronome.h"

class Module
{
 private:
  LeapMux *mux;
  int channel;
 protected:
  LeapPd *pd;

 public:
 Module(LeapMux *mux, LeapPd *pd, int channel) :
  mux(mux), channel(channel), pd(pd)
  {};
  virtual bool update(MetronomeState state) = 0;
  virtual void rapidUpdate(int msPassed) { }
  virtual void changeInstrument(int instrumentNumber) { };
  virtual void incrementKey() { };
  virtual void decrementKey() { };

 protected:
  HandState hand(bool forceCurrent = false) {
    return mux->currentGesture(channel, forceCurrent);
  };
};

#endif
