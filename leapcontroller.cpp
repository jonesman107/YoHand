#include <mutex>
#include <iostream>
#include <vector>
#include <cmath>

#include "Leap.h"
#include "leapcontroller.h"

#define E 2.71828
#define SOFTMAX_FACTOR 6.5

using namespace Leap;
using namespace std;

float clamp(float val, float min, float max);

LeapController::LeapController()
{
  ready_ = false;
  controller_.addListener(*this);
}

LeapController::~LeapController()
{
  // TODO
}

void LeapController::onInit(const Controller&)
{
  // TODO
}

void LeapController::onConnect(const Controller&)
{
  cout << "Connected to a Leap Motion successfully." << endl;

  controller_.enableGesture(Gesture::TYPE_CIRCLE);
  controller_.enableGesture(Gesture::TYPE_SWIPE);
  ready_ = true;
}

void LeapController::onDisconnect(const Controller&)
{
  ready_ = false;
}

void LeapController::onExit(const Controller&)
{
  // TODO
}

void LeapController::onFrame(const Controller& controller)
{
  const Frame frame = controller.frame();
  /*
  const GestureList gestures = frame.gestures();
  for (int i = 0; i < gestures.count(); i++) {
    Gesture g = gestures[i];
    cout << g.toString() << endl;
  }
  */

  lock_guard<mutex> guard(mutex_);
  prev_gesture_ = cur_gesture_;
  cur_gesture_.clear();

  HandState state;
  HandList hands = frame.hands();
  for (auto h : hands) {
    state.left = h.isLeft();
    state.handid = h.id();

    Vector pos = h.palmPosition();
    pos = Vector(pos.x / 2, pos.y, pos.z);
    pos = frame.interactionBox().normalizePoint(pos);
    const Vector norm = h.palmNormal();
    const Vector dir = h.direction();
    state.x = pos.x;
    state.y = pos.y;
    state.z = pos.z;
    state.nx = norm.x;
    state.ny = norm.y;
    state.nz = norm.z;
    state.dirx = dir.x;
    state.diry = dir.y;
    state.dirz = dir.z;

    state.numExtended = 0;

    bool index, pinky;
    index = pinky = false;
    Vector indexDir, pinkyDir;

    for (int i = 0; i < h.fingers().count(); i++) {
      auto cf = h.fingers()[i];
      // cidx guarantees fpos[0] = thumb, fpos[1] = index,
      // fpos[2] = middle, fpos[3] = ring, fpos[4] = pinky
      int cidx = cf.type();
      
      Vector pos = cf.tipPosition();
      pos = Vector(pos.x / 2, pos.y, pos.z);

      if (cidx == 1) {
	index = true;
	indexDir = cf.direction();
      } else if (cidx == 4) {
	pinky = true;
	pinkyDir = cf.direction();
      }

      auto fpos = frame.interactionBox().normalizePoint(pos);
      if (cf.isExtended()) {
        state.numExtended++;
        state.fpos[cidx][0] = fpos.x;
        state.fpos[cidx][1] = fpos.y;
        state.fpos[cidx][2] = fpos.z;
      } else {
        state.fpos[cidx][0] = state.fpos[cidx][1] = state.fpos[cidx][2] = 0;
      }
      /* state.fpos[cidx][0] = fpos.x;
      state.fpos[cidx][1] = fpos.y;
      state.fpos[cidx][2] = fpos.z; */
    }

    float point, pinch, flat, fist, splay;
    point = pinch = flat = fist = splay = 0;
    fist = h.grabStrength();
    flat = 1 - h.grabStrength();
    pinch = h.pinchStrength();
    if (fist > pinch / 2)
      fist -= pinch / 2;
    if (index && pinky) {
      splay = 4 * pow(1 - clamp(indexDir.dot(pinkyDir), 0, 1), .5) * flat;
    }

    // softmax
    point = 0; //pow(E, SOFTMAX_FACTOR * point);
    pinch = 0; //pow(E, SOFTMAX_FACTOR * pinch);
    flat  = pow(E, SOFTMAX_FACTOR * flat);
    fist  = pow(E, SOFTMAX_FACTOR * fist);
    splay = pow(E, SOFTMAX_FACTOR * splay);
    float total = point + pinch + flat + fist + splay;
    point /= total;
    pinch /= total;
    flat  /= total;
    fist  /= total;
    splay /= total;

    float c  = 1;
    float nc = 0;
    HandState prev_state;
    for (auto prev : prev_gesture_) {
      if (prev.handid == state.handid) {
    prev_state = prev;
    c  = h.confidence();
    nc = 1 - c;
    break;
      }
    }
    state.point = c * point + nc * prev_state.point;
    state.pinch = c * pinch + nc * prev_state.pinch;
    state.flat  = c * flat  + nc * prev_state.flat;
    state.fist  = c * fist  + nc * prev_state.fist;
    state.splay = c * splay + nc * prev_state.splay;
    cur_gesture_.push_back(state);
  }
}

void LeapController::onFocusGained(const Controller&)
{
  // TODO
  cout << "FocusGained" << endl;
}

void LeapController::onFocusLost(const Controller&)
{
  // TODO
  cout << "FocusLost" << endl;
}

void LeapController::onDeviceChange(const Controller&)
{
  // TODO
}

void LeapController::onServiceConnect(const Controller&)
{
  // TODO
}

void LeapController::onServiceDisconnect(const Controller&)
{
  // TODO
}

vector<HandState> LeapController::currentGesture()
{
  lock_guard<mutex> guard(mutex_);
  return cur_gesture_;
}
