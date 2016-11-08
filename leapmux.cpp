#include <vector>
#include <iostream>

#include "leapcontroller.h"

#include "leapmux.h"

using namespace std;

#define CHECK_CHANNEL if (channel > maxchannel) {	     \
    cerr << "Requested an invalid gesture channel." << endl; \
    exit(1);						     \
  }


struct PlaybackState {
  vector<HandState> hs;
  int hand;
  bool playing;
  int playPos;
  bool loop;
  bool frozen;
  HandState lastState;
};

LeapMux::LeapMux(LeapController *lp, int channels) :
  lp(lp), channels(channels)
{
  states = new PlaybackState[channels];
  for (int i = 0; i < channels; i++) {
    states[i].playing = false;
    states[i].frozen = false;

    states[i].playPos = 0;
    states[i].loop = false;
  }

  maxchannel = -1;
}

LeapMux::~LeapMux()
{
  delete [] states;
}

int LeapMux::newChannel(int hand)
{
  if (maxchannel == channels)
    return -1;
  int channel = ++maxchannel;

  states[channel].hs.clear();
  states[channel].hand = hand;
  states[channel].playing = false;
  states[channel].playPos = 0;
  states[channel].loop = false;
  states[channel].lastState = defaultState(hand);

  return channel;
}

HandState LeapMux::currentGesture(int channel, bool force_actual)
{
  CHECK_CHANNEL;

  if (states[channel].playing) {
    // TODO: Playback
    return defaultState(states[channel].hand);
  } else {
    vector<HandState> hs = lp->currentGesture();
    HandState state = defaultState(states[channel].hand);
    for (auto h : hs) {
      if ((states[channel].hand == LEFT_HAND && h.left) ||
    (states[channel].hand == RIGHT_HAND && !h.left)) {
        if (!h.left) {
          h.x = max(0.0, h.x - 0.5);
          for (int i = 0; i < 5; i++) {
            h.fpos[i][0] = max(0.0, h.fpos[i][0] - 0.5);
          }
        }
        h.x = min(1.0, h.x * 2.0);
        for (int i = 0; i < 5; i++) {
          h.fpos[i][0] = min(1.0, 2.0 * h.fpos[i][0]);
        }

  state = h;
  break;
      }
    }

    if ((state.gesture() == G_FIST || state.gesture() == G_PINCH)) {
      states[channel].frozen = true;
      if (force_actual) return state;
      else return states[channel].lastState;
    } else {
      states[channel].frozen = false;
    }
    states[channel].lastState = state;

    // TODO: Record
    return state;
  }
}

bool LeapMux::isFrozen(int channel)
{
  CHECK_CHANNEL;
  return states[channel].frozen;
}

void LeapMux::record(int channel)
{
  CHECK_CHANNEL;
  // TODO
}

void LeapMux::stopRecord(int channel)
{
  CHECK_CHANNEL;
  // TODO
}

void LeapMux::play(int channel, bool loop)
{
  CHECK_CHANNEL;
  // TODO
}

void LeapMux::stopPlay(int channel)
{
  CHECK_CHANNEL;
  // TODO
}

HandState LeapMux::defaultState(int hand)
{
  HandState state;
  state.isPresent = false;

  state.left = (hand == LEFT_HAND);
  state.handid = -1;
  state.x = state.y = state.z = 0;

  state.nx = 0;
  state.ny = -1;
  state.nz = 0;
  state.dirx = 0;
  state.diry = 0;
  state.dirz = -1;

  state.point = state.pinch = state.flat = state.splay = 0;
  state.fist = 1;
  return state;
}
