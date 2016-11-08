#include <iostream>
#include <algorithm>

#include "leappd.h"
#include "PdBase.hpp"
#include "leapcontroller.h"
#include "rtaudio.h"

using namespace std;
using namespace pd;

PdBase lpd;

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData){
      // Pd magic
      int ticks = nBufferFrames / 64;
     lpd.processFloat(ticks, (float *)inputBuffer, (float*)outputBuffer);
    return 0;
}

void LeapPdReceiver::print(const string &msg)
{
//  cout << msg << endl;
}

void LeapPdReceiver::receiveBang(const string &dst)
{
  cout << "BANG!!!!" << endl;
}

void LeapPdReceiver::receiveFloat(const string &dst, float num)
{

}

void LeapPdReceiver::receiveSymbol(const string &dst, const string &sym)
{

}

void LeapPdReceiver::receiveMessage(const string &dist, const string &msg, const List &list)
{

}

void LeapPdReceiver::receiveNoteOn(const int channel, const int pitch, const int velocity) {
    cout << "CPP MIDI: note on: " << channel << " " << pitch << " " << velocity << endl;
}

void LeapPdReceiver::receiveControlChange(const int channel, const int controller, const int value) {
    cout << "CPP MIDI: control change: " << channel << " " << controller << " " << value << endl;
}

void LeapPdReceiver::receiveProgramChange(const int channel, const int value) {
    cout << "CPP MIDI: program change: " << channel << " " << value << endl;
}

void LeapPdReceiver::receivePitchBend(const int channel, const int value) {
    cout << "CPP MIDI: pitch bend: " << channel << " " << value << endl;
}

void LeapPdReceiver::receiveAftertouch(const int channel, const int value) {
    cout << "CPP MIDI: aftertouch: " << channel << " " << value << endl;
}

void LeapPdReceiver::receivePolyAftertouch(const int channel, const int pitch, const int value) {
    cout << "CPP MIDI: poly aftertouch: " << channel << " " << pitch << " " << value << endl;
}

void LeapPdReceiver::receiveMidiByte(const int port, const int byte) {
    cout << "CPP MIDI: midi byte: " << port << " " << byte << endl;
}

LeapPd::LeapPd(const string &path)
{
  int srate = 44100;
  unsigned int bufferFrames = 128;

  if (!lpd.init(0,2, srate, true)) {
    cerr << "Could not initialize pd." << endl;
    exit(1);
  }
  
  lpd.subscribe("env");

  lpd.setReceiver(&receiver_);
  lpd.setMidiReceiver(&receiver_);
  lpd.addToSearchPath("pd");
  lpd.computeAudio(true);


  patch_ = lpd.openPatch(path, "pd");
  cout << "Building " << patch_ << endl;

  if (audio_.getDeviceCount() == 0) {
    cerr << "No sound output available." << endl;
    exit(1);
  }

  RtAudio::StreamParameters parameters;
  parameters.deviceId = audio_.getDefaultOutputDevice();
  parameters.nChannels = 2;

  RtAudio::StreamOptions options;
  options.streamName = "LeapPD";
  options.flags = RTAUDIO_SCHEDULE_REALTIME;
  if (audio_.getCurrentApi() != RtAudio::MACOSX_CORE) {
    options.flags |= RTAUDIO_MINIMIZE_LATENCY;
  }
  try {
    audio_.openStream(&parameters, NULL, RTAUDIO_FLOAT32, srate, &bufferFrames,
		      &audioCallback, NULL, &options);
    audio_.startStream();
  } catch (RtAudioError &e) {
    cerr << "Error opening RtAudio stream." << endl;
    cerr << e.getMessage() << endl;
    exit(1);
  }
}

LeapPd::~LeapPd()
{
  lpd.closePatch(patch_);
}

void LeapPd::sendHand(const HandState &hg)
{
  lpd.receiveMessages();
  // TODO (only if we need this)
  tick();
}

void LeapPd::sendFloat(string name, float value)
{
  lpd.sendFloat(name, value);
  tick();
}

void LeapPd::sendMidi(int channel, int pitch, int velocity)
{
  lpd.sendNoteOn(channel, pitch, velocity);
  tick();
}

void LeapPd::tick()
{
  lpd.receiveMidi();
  lpd.receiveMessages();
}
