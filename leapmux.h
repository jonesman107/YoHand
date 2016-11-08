#ifndef LEAPMUX_H_
#define LEAPMUX_H_

#include "leapcontroller.h"

struct PlaybackState;

#define LEFT_HAND 0
#define RIGHT_HAND 1

class LeapMux
{
 private:
  LeapController *lp;
  int channels;
  PlaybackState *states;
  int maxchannel;
  bool *frozen;
  
 public:
  LeapMux(LeapController *lp, int maxchannels);
  virtual ~LeapMux();

  int newChannel(int hand);

  bool isFrozen(int channel);
  HandState currentGesture(int channel, bool force_actual = false);
  
  void record(int channel);
  void stopRecord(int channel);
  
  void play(int channel, bool loop);
  void stopPlay(int channel);

 private:
  HandState defaultState(int hand);
};

#endif
