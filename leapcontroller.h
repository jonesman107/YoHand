#ifndef LEAPCONTROLLER_H
#define LEAPCONTROLLER_H

#include <mutex>
#include <vector>
#include <cmath>

#include "Leap.h"

using namespace std;
using namespace Leap;

typedef enum {
  G_POINT,
  G_PINCH,
  G_FLAT,
  G_FIST,
  G_SPLAY,
  G_UNKNOWN
} GestureType;

struct HandState {
  bool isPresent = true;

  bool left;
  int handid;

  float x, y, z;
  float nx, ny, nz;
  float dirx, diry, dirz;
  float point;
  float pinch;
  float flat;
  float fist;
  float splay;
  float fpos[5][3];
  int numExtended;

  bool operator<(HandState &o) { return handid < o.handid; };

  string toString()
  {
    switch (this->gesture()) {
      case G_POINT:
        return "point";
      case G_PINCH:
        return "pinch";
      case G_FLAT:
        return "flat";
      case G_FIST:
        return "fist";
      case G_SPLAY:
        return "splay";
      default:
        return "unsure";
    }
  };

  float maxWeight()
  {
    return max(point, max(pinch, max(flat, max(fist, splay))));
  }

  GestureType gesture()
  {
    if (point > pinch && point > flat && point > fist && point > splay)
      return G_POINT;
    else if (pinch > point && pinch > flat && pinch > fist && pinch > splay)
      return G_PINCH;
    else if (flat > point && flat > pinch && flat > fist && flat > splay)
      return G_FLAT;
    else if (fist > point && fist > pinch && fist > flat && fist > splay)
      return G_FIST;
    else if (splay > point && splay > pinch && splay > flat && splay > fist)
      return G_SPLAY;
    else
      return G_UNKNOWN;
  }
};

class LeapController : public Leap::Listener {
private:
  Controller controller_;
  mutex mutex_;
  vector<HandState> cur_gesture_;
  vector<HandState> prev_gesture_;
  bool ready_;
public:
  LeapController();
  virtual ~LeapController();

  virtual bool ready() { return ready_; }

  virtual void onInit(const Controller&);
  virtual void onConnect(const Controller&);
  virtual void onDisconnect(const Controller&);
  virtual void onExit(const Controller&);
  virtual void onFrame(const Controller&);
  virtual void onFocusGained(const Controller&);
  virtual void onFocusLost(const Controller&);
  virtual void onDeviceChange(const Controller&);
  virtual void onServiceConnect(const Controller&);
  virtual void onServiceDisconnect(const Controller&);

  virtual vector<HandState> currentGesture();
};

#endif // LEAPCONTROLLER_H
