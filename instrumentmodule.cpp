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

InstrumentModule::InstrumentModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
    Module(mux, pd, channel), gfx(gfx)
{
    prevInstrument = -1;
    currentInstrument = 3;
    checkInstrumentChange();
    gfx->sendInstrumentNumber(3);
    gfx->sendHarmony(&harmony);
    melodyNote = -1;
    chordChange[0] = chordChange[1] = chordChange[2] = chordChange[3] = curPos = 0;
    curSubdiv = SIXTEENTH_SUBDIV;
}

void InstrumentModule::sendNote(int channel, int note, int velocity, int *updateNote)
{
    if (note >= 0 && note < harmony.NUM_MIDI_NOTES) {
        pd->sendMidi(channel, note, velocity);
        gfx->sendNote(note, velocity);
    }
    if (updateNote)
        *updateNote = (velocity == NOTE_OFF) ? -1 : note;
    std::cout << "send note channel " << channel << std::endl;
}

static int gestureVote = 0;
bool InstrumentModule::update(MetronomeState state)
{

    checkInstrumentChange();
    if (checkStop()) return false;
    updateNotesIfNeeded(state);
    //  sendChordsIfNeeded(state);
    sendMelody(state);
    return true;
}

void InstrumentModule::sendMelody(MetronomeState state) {
    HandState h = hand();

    if ((h.splay < 0.05 && state.quarter) ||
            (h.splay >= 0.05 && h.splay < 0.35 && state.eighth) ||
            (h.splay >= 0.35 && h.splay < 0.75 && state.eighthTriplet) ||
            (h.splay >= 0.75 && state.sixteenth)) {
        int beatNum = state.sixteenthNum;
        int velo = QUARTER_VELO;
        if (beatNum % 2) {
            velo = SIXTEENTH_VELO;
        } else if (beatNum % 4) {
            velo = EIGHTH_VELO;
        }
        int curSixteenth = sequence[curPos];
        if (curPos == 0 || sequence[curPos - 1] != curSixteenth ) {
            //            sendNote(SYNTH_CHANNEL, melodyNote, NOTE_OFF, NULL);
            //            sendNote(SYNTH_CHANNEL, melodyNote - 12, NOTE_OFF, NULL);
            //            sendNote(SYNTH_CHANNEL, curSixteenth, velo, &melodyNote);
            //            sendNote(SYNTH_CHANNEL, curSixteenth - 12, velo, NULL);
            curPos = (curPos + 1) % curSubdiv;
        }
    }
}

void InstrumentModule::updateNotesIfNeeded(MetronomeState state) {
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
}

bool InstrumentModule::checkStop() {
    HandState h = hand(true);
    if (h.gesture() == G_FIST || h.gesture() == G_PINCH) {
        gestureVote++;
        if (gestureVote > 5) {
            stopAllNotes();
            gestureVote = 0;
            return true;
        }
    } else {
        gestureVote = max(gestureVote - 2, 0);
    }
    return false;
}

void InstrumentModule::stopAllNotes() {
    for (int i = 0; i < 3; i++) {
        sendNote(chordChannel, prevTriad[i], NOTE_OFF, NULL);
    }
    sendNote(SYNTH_CHANNEL, melodyNote, NOTE_OFF, NULL);
}

void InstrumentModule::sendEnvelope(int attack, int decay, float sustain, int release) {
    pd->sendFloat("bs_a", attack);
    pd->sendFloat("bs_d", decay);
    pd->sendFloat("bs_s", sustain);
    pd->sendFloat("bs_r", release);
    gfx->setSynth(attack, decay, sustain, release);
}

void InstrumentModule::sendCompressor(int instrumentNumber) {
    if (instrumentNumber == 10)
        pd->sendFloat("$0-gain", -2);
    else if (instrumentNumber == 11)
        pd->sendFloat("$0-gain", 10);
    else
        pd->sendFloat("$0-gain", 10);

    pd->sendFloat("$0-response", 50);
    pd->sendFloat("$0-thresh", -6);
    pd->sendFloat("$0-ratio", 50);
    pd->sendFloat("$0-lookahead", 50);
}

void InstrumentModule::checkInstrumentChange() {
    if (prevInstrument == currentInstrument) return;
    if (0 <= currentInstrument && currentInstrument <= 9) {
        chordChannel = MELLOTRON_CHANNEL;
        pd->sendFloat("bs_instru_type", currentInstrument);
    } else if (currentInstrument <= 11) {
        chordChannel = SYNTH_CHANNEL;
        int wave = (currentInstrument == 11);
        pd->sendFloat("bs_wave_shape", wave);
    }

    int min_ = harmony.MIN_NOTE - 12;
    for (int i = min_; i < min_ + harmony.RANGE * 12 + 12; i++) {
        sendNote(MELLOTRON_CHANNEL, i, NOTE_OFF, NULL);
        sendNote(SYNTH_CHANNEL, i, NOTE_OFF, NULL);
    }
    prevInstrument = currentInstrument;
}

void InstrumentModule::changeInstrument(int instrumentNumber) {
    currentInstrument = instrumentNumber;
}

void InstrumentModule::incrementKey() {
    harmony.changeKeyIfScheduled(1);
    gfx->sendKey(harmony.getKey());
}

void InstrumentModule::decrementKey() {
    harmony.changeKeyIfScheduled(-1);
    gfx->sendKey(harmony.getKey());
}
