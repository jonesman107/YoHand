#ifndef INSTRUMENTMODULE_H
#define INSTRUMENTMODULE_H


#include "module.h"
#include "graphics.h"
#include "metronome.h"

class InstrumentModule : public Module
{
public:
    InstrumentModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel);
    bool update(MetronomeState state);
    void changeInstrument(int instrumentNumber);
    void sendEnvelope(int attack, int decay, float sustain, int release);
    void sendCompressor(int instrumentNumber);
    void incrementKey();
    void decrementKey();


protected:
    Graphics *gfx;
    int prevRoot;
    int prevTriad[3];
    int curSubdiv;
    int curPos;
    int chordChannel;

    void sendNote(int channel, int note, int velocity, int *updateNote);
    void stopAllNotes();

private:
    int sequence[16];
    int melodyNote;
    int chordChange[4];
    int changeKey;
    int prevInstrument;
    int currentInstrument;

    void checkInstrumentChange();
    bool checkStop();
    void updateCurrentSubdivision();
    void sendMelody(MetronomeState state);
    void updateNotesIfNeeded(MetronomeState state);

};

#endif // INSTRUMENTMODULE_H
