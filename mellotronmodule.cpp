#include "mellotronmodule.h"
#include "instrumentmodule.h"
#include "musictheory.h"
#include "graphics.h"
#include "audiowidget.h"

#define QUARTER_VELO 100
#define EIGHTH_VELO 80
#define SIXTEENTH_VELO 50
#define CHORD_VELO 65
#define SYNTH_CHANNEL 0
#define MELLOTRON_CHANNEL 1

#define NOTE_OFF 0
static Harmony harmony;

MellotronModule::MellotronModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
    InstrumentModule(mux, pd, gfx, channel)
{
    prevTriad[0] = prevTriad[1] = prevTriad[2] = -1;
    //  chordChange[0] = chordChange[1] = chordChange[2] = chordChange[3] = curPos = 0;
    //  curSubdiv = SIXTEENTH_SUBDIV;
}

bool MellotronModule::update(MetronomeState state)
{

    if (!InstrumentModule::update(state)) return false;
    std::cout << "Mellotron Module update: " << curPos << std::endl;
    //  updateNotesIfNeeded(state);
    sendChordsIfNeeded(state);
    return true;
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
        harmony.getTriad(currentRoot, rand() % 2, triad, prevTriad);
        sendNote(chordChannel, triad[0], CHORD_VELO, &prevTriad[0]);
        sendNote(chordChannel, triad[1], CHORD_VELO, &prevTriad[1]);
        sendNote(chordChannel, triad[2], CHORD_VELO, &prevTriad[2]);
        // update the root note UI element
        gfx->sendRootNote(currentRoot - harmony.BUFFER, harmony.NOTES_PER_KEY);
        prevRoot = currentRoot;
        delete[] triad;
    }
}
