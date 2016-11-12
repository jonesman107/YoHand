#include "synthmodule.h"
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

SynthModule::SynthModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
    InstrumentModule(mux, pd, gfx, channel)
{
    melodyNote = prevInstrument = -1;
    chordChange[0] = chordChange[1] = chordChange[2] = chordChange[3] = curPos = 0;
    curSubdiv = SIXTEENTH_SUBDIV;
    currentInstrument = 11;
    checkInstrumentChange();
}

void SynthModule::update(MetronomeState state)
{
    checkInstrumentChange();

    if (checkStop()) {
        stopAllNotes();
        return;
    }

//    updateNotesIfNeeded(state);
    sendMelody(state);
}

void SynthModule::sendMelody(MetronomeState state) {
    HandState h = hand();
    int currentMelodyNote = harmony.getNoteInKeyAtHeight(h.y);
    std::cout << currentMelodyNote << std::endl;
    if (currentMelodyNote != melodyNote) {
        stopAllNotes();
        sendNote(SYNTH_CHANNEL_R, currentMelodyNote, EIGHTH_VELO, &melodyNote);
//        sendNote(SYNTH_CHANNEL_R, currentMelodyNote - 12, EIGHTH_VELO, NULL);
    }

//    if ((h.splay < 0.05 && state.quarter) ||
//        (h.splay >= 0.05 && h.splay < 0.35 && state.eighth) ||
//        (h.splay >= 0.35 && h.splay < 0.75 && state.eighthTriplet) ||
//        (h.splay >= 0.75 && state.sixteenth)) {
//        int beatNum = state.sixteenthNum;
//        int velo = QUARTER_VELO;
//        if (beatNum % 2) {
//            velo = SIXTEENTH_VELO;
//        } else if (beatNum % 4) {
//            velo = EIGHTH_VELO;
//        }
//        int curSixteenth = sequence[curPos];
//        if (curPos == 0 || sequence[curPos - 1] != curSixteenth ) {
//            sendNote(SYNTH_CHANNEL, melodyNote, NOTE_OFF, NULL);
//            sendNote(SYNTH_CHANNEL, melodyNote - 12, NOTE_OFF, NULL);
//            sendNote(SYNTH_CHANNEL, curSixteenth, velo, &melodyNote);
//            sendNote(SYNTH_CHANNEL, curSixteenth - 12, velo, NULL);
//            curPos = (curPos + 1) % curSubdiv;
//        }
//    }
}



void SynthModule::updateNotesIfNeeded(MetronomeState state) {
    if (!state.quarter) return;
    HandState h = hand();
    int beatNum = state.quarterNum;
    int currentRoot = harmony.getRootNote(h.y);
    if (state.measure || currentRoot != prevRoot) {
        int allocated = 0, nextMax = 4;
        for (int i = 3; i > beatNum; i--) {
            if (rand() % 2) {
                chordChange[i] = nextMax;
                allocated += 4;
                nextMax = 4;
            } else {
                chordChange[i] = 0;
                nextMax += 4;
            }
        }
        chordChange[beatNum] = nextMax;
    }

    if (chordChange[beatNum]) {
        harmony.getNoteSequence(currentRoot, h.nx, sequence, chordChange[beatNum]);
        curSubdiv = chordChange[beatNum];
        curPos = 0;
    }
    prevRoot = currentRoot;
}

void SynthModule::stopAllNotes() {
    sendNote(SYNTH_CHANNEL_R, melodyNote, NOTE_OFF, NULL);
//    sendNote(SYNTH_CHANNEL_R, melodyNote - 12, NOTE_OFF, NULL);
}

void SynthModule::sendEnvelope(int attack, int decay, float sustain, int release) {
    pd->sendFloat("bs_a", attack);
    pd->sendFloat("bs_d", decay);
    pd->sendFloat("bs_s", sustain);
    pd->sendFloat("bs_r", release);

    pd->sendFloat("bs_voice_filt_on", 1);
    pd->sendFloat("bs_start", 8500);
    pd->sendFloat("bs_end", 300);
    pd->sendFloat("bs_time", 6000);
    pd->sendFloat("bs_q", 5);
}

void SynthModule::checkInstrumentChange() {
    if (prevInstrument == currentInstrument) return;
//    if (currentInstrument > 9)
//        pd->sendFloat("bs_wave_shape", currentInstrument % 10);

    pd->sendFloat("bs_wave_shape_r", 0);

    sendCompressor(currentInstrument);
    sendEnvelope(100, 1000, 1, 100);
    stopAllNotes();
    prevInstrument = currentInstrument;
}

