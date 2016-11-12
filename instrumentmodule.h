#ifndef INSTRUMENTMODULE_H
#define INSTRUMENTMODULE_H


#include "module.h"
#include "graphics.h"
#include "metronome.h"
#include "musictheory.h"

#define QUARTER_VELO 100
#define EIGHTH_VELO 80
#define SIXTEENTH_VELO 50
#define CHORD_VELO 65
#define SYNTH_CHANNEL_L 0
#define MELLOTRON_CHANNEL_L 1
#define SYNTH_CHANNEL_R 2
#define MELLOTRON_CHANNEL_R 3
#define NOTE_OFF 0

class InstrumentModule : public Module
{

public:


    InstrumentModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
    void update(MetronomeState state);
    void changeInstrument(int instrumentNumber);
    void sendCompressor(int instrumentNumber);
    void incrementKey();
    void decrementKey();


protected:
    Graphics *gfx;
    Harmony harmony;
    int gestureVote = 0;
    int prevRoot;
    int prevInstrument;
    int currentInstrument;

    void sendNote(int channel, int note, int velocity, int *updateNote);
    bool checkStop();
    void stopAllNotes();

private:
    int changeKey;


};

#endif // INSTRUMENTMODULE_H
