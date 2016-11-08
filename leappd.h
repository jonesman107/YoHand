#ifndef LEAP_PD_H_
#define LEAP_PD_H_

#include <string>

#include "PdBase.hpp"
#include "leapcontroller.h"
#include "rtaudio.h"

using namespace std;
using namespace pd;

class LeapPdReceiver : public PdReceiver, public PdMidiReceiver {
 public:
  // Override these as needed!
  virtual void print(const string &msg);
  virtual void receiveBang(const string &dst);
  virtual void receiveFloat(const string &dst, float num);
  virtual void receiveSymbol(const string &dst, const string &sym);
  virtual void receiveMessage(const string &dist, const string &msg, const List &list);

    // pd midi receiver callbacks
    virtual void receiveNoteOn(const int channel, const int pitch, const int velocity);
    virtual void receiveControlChange(const int channel, const int controller, const int value);
    virtual void receiveProgramChange(const int channel, const int value);
    virtual void receivePitchBend(const int channel, const int value);
    virtual void receiveAftertouch(const int channel, const int value);
    virtual void receivePolyAftertouch(const int channel, const int pitch, const int value);

    virtual void receiveMidiByte(const int port, const int byte);
};

class LeapPd {
private:
  float inbuf_[64], outbuf_[128];
  LeapPdReceiver receiver_;
//  PdMidiReceiver midi_;
  Patch patch_;
  RtAudio audio_;
  
public:
  LeapPd(const string &path);
  virtual ~LeapPd();

  virtual void sendHand(const HandState &hg);
  virtual void sendFloat(string name, float value);
  virtual void sendMidi(int channel, int pitch, int velocity);
  virtual void tick();
};


#endif // LEAP_PD_H_
