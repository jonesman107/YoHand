#include "instrumentmodule.h"
#include "musictheory.h"
#include "graphics.h"
#include "audiowidget.h"


Harmony harmony;

InstrumentModule::InstrumentModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
    Module(mux, pd, channel), gfx(gfx)
{
    prevInstrument = -1;
//    pd->sendFloat("bs_wave_shape", 1);
    gfx->sendInstrumentNumber(0);
    gfx->sendHarmony(&harmony);
}

void InstrumentModule::sendNote(int channel, int note, int velocity, int *updateNote)
{
    if (note >= 0 && note < harmony.NUM_MIDI_NOTES) {
        pd->sendMidi(channel, note, velocity);
        gfx->sendNote(note, velocity);
    }
    if (updateNote)
        *updateNote = (velocity == NOTE_OFF) ? -1 : note;
//    std::cout << "send note channel " << channel << std::endl;
}

void InstrumentModule::update(MetronomeState state)
{
//    checkInstrumentChange();
//    pd->sendFloat("bs_volume", 0.5);
}

bool InstrumentModule::checkStop() {
    HandState h = hand(true);
    if (h.gesture() == G_FIST || h.gesture() == G_PINCH) {
        gestureVote++;
        if (gestureVote > 5) {
            gestureVote = 0;
            return true;
        }
    } else {
        gestureVote = max(gestureVote - 2, 0);
    }
    return false;
}

void InstrumentModule::sendCompressor(int instrumentNumber) {
    if (instrumentNumber == 10)
        pd->sendFloat("$0-gain", -2);
    else if (instrumentNumber == 11)
        pd->sendFloat("$0-gain", 10);
    else
        pd->sendFloat("$0-gain", 10);

    pd->sendFloat("$0-response", 40);
    pd->sendFloat("$0-thresh", -2);
    pd->sendFloat("$0-ratio", 50);
    pd->sendFloat("$0-lookahead", 50);
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
