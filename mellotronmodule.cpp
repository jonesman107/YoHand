#include "mellotronmodule.h"
#include "instrumentmodule.h"
#include "musictheory.h"
#include "graphics.h"
#include "audiowidget.h"

#define QUARTER_VELO 100
#define EIGHTH_VELO 80
#define SIXTEENTH_VELO 50
#define CHORD_VELO 65
#define SYNTH_CHANNEL_L 0
#define MELLOTRON_CHANNEL_L 1
#define SYNTH_CHANNEL_R 2
#define MELLOTRON_CHANNEL_R 3
#define NOTE_OFF 0

MellotronModule::MellotronModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
    InstrumentModule(mux, pd, gfx, channel)
{
    prevTriad[0] = prevTriad[1] = prevTriad[2] = prevRoot = prevInstrument = -1;
    currentInstrument = 0;
    checkInstrumentChange();
}

void MellotronModule::update(MetronomeState state)
{
    checkInstrumentChange();

    if (InstrumentModule::checkStop()) {
        stopAllNotes();
        return;
    }

    sendChordsIfNeeded(state);
}

void MellotronModule::checkInstrumentChange() {
    if (prevInstrument == currentInstrument) return;
//    if (0 <= currentInstrument && currentInstrument <= 9)
//        pd->sendFloat("bs_instru_type", currentInstrument);

    pd->sendFloat("bs_wave_shape", 1);

    stopAllNotes();
    prevInstrument = currentInstrument;
}

void MellotronModule::sendChordsIfNeeded(MetronomeState state)
{
    if (!state.quarter) return;
    HandState h = hand();
    int currentRoot = harmony.getRootNote(h.y);
    int beatNum = state.quarterNum;
    if (prevRoot != currentRoot) {
        stopAllNotes();
        int *triad = new int[3];
        bool openChord = rand() % 3;
        harmony.getTriad(currentRoot, h.splay < 0.4, triad, prevTriad);
        sendNote(SYNTH_CHANNEL_L, triad[0], CHORD_VELO, &prevTriad[0]);
        sendNote(SYNTH_CHANNEL_L, triad[1], CHORD_VELO, &prevTriad[1]);
        sendNote(SYNTH_CHANNEL_L, triad[2], CHORD_VELO, &prevTriad[2]);
        // update the root note UI element
        gfx->sendRootNote(currentRoot - harmony.BUFFER, harmony.NOTES_PER_KEY);
        prevRoot = currentRoot;
        delete[] triad;
    }
}

void MellotronModule::stopAllNotes() {
    sendNote(SYNTH_CHANNEL_L, prevTriad[0], NOTE_OFF, NULL);
    sendNote(SYNTH_CHANNEL_L, prevTriad[1], NOTE_OFF, NULL);
    sendNote(SYNTH_CHANNEL_L, prevTriad[2], NOTE_OFF, NULL);
}
