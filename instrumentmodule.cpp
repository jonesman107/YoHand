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

//Rhythm::Rhythm()
//{
//  clearAccents();
//}

//void Rhythm::incrAccent(MetronomeState state)
//{
//  int off = state.sixteenthTripletNum;
//  if (!accents[off].beat ||
//      accents[off].measureChanged == state.measuresElapsed)
//    return;

//  if (accents[off].type == BEAT_QUARTER) {
//    int change = off + EIGHTH_SUBDIV;
//    accents[off].measureChanged = state.measuresElapsed;
//    accents[off].type = BEAT_EIGHTH;
    
//    accents[change].beat = true;
//    accents[change].measureChanged = state.measuresElapsed;
//    accents[change].type = BEAT_EIGHTH;
//  } else if (accents[off].type == BEAT_EIGHTH) {
//    int change = off + SIXTEENTH_SUBDIV;
//    accents[off].measureChanged = state.measuresElapsed;
//    accents[off].type = BEAT_SIXTEENTH;
    
//    accents[change].beat = true;
//    accents[change].measureChanged = state.measuresElapsed;
//    accents[change].type = BEAT_SIXTEENTH;
//  } else if (accents[off].type == BEAT_SIXTEENTH) {
//    accents[off].measureChanged = state.measuresElapsed;
//    accents[off].type = BEAT_SIXTRIPLE;
//    accents[off+1].beat = true;
//    accents[off+1].measureChanged = state.measuresElapsed;
//    accents[off+1].type = BEAT_SIXTRIPLE;
//    accents[off+2].beat = true;
//    accents[off+2].measureChanged = state.measuresElapsed;
//    accents[off+2].type = BEAT_SIXTRIPLE;
//  }
  
//}

//void Rhythm::clearAccents()
//{
//  for (int i = 0; i < MAX_SUBDIV; i++) {
//    if (i % QUARTER_SUBDIV)
//      accents[i].beat = false;
//    else {
//      accents[i].beat = true;
//      accents[i].measureChanged = -1;
//      accents[i].type = BEAT_QUARTER;
//    }
//  }
//}

//bool Rhythm::accentAt(MetronomeState state)
//{
//  return accents[state.sixteenthTripletNum].beat;
//}

//int Rhythm::numAccents()
//{
//  return MAX_SUBDIV;
//}

//void Rhythm::getAccents(bool *arr)
//{
//  for (int i = 0; i < MAX_SUBDIV; i++) {
//    arr[i] = accents[i].beat;
//  }
//}

//void InstrumentModule::updateRhythm(MetronomeState state)
//{
//    HandState h = hand();
//    // Calculate rhythm.
//    float forward_tilt = h.nz / h.ny;
//    if (forward_tilt > 1) {
//      rhythm.incrAccent(state);
//    } else if (forward_tilt < -1) {
//      rhythm.clearAccents();
//    }
//    /*
//    if (state.sixteenthTripletNum % (4 * 3) == 0) cout << endl;
//    if (rhythm.accentAt(state)) cout << "." << flush;
//    else cout << " " << flush;
//    */
//}

InstrumentModule::InstrumentModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
  Module(mux, pd, channel), gfx(gfx)
{
  prevInstrument = -1;
  currentInstrument = 3;
  checkInstrumentChange();
  gfx->sendInstrumentNumber(3);
  gfx->sendHarmony(&harmony);
  prevTriad[0] = prevTriad[1] = prevTriad[2] = melodyNote = -1;
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
  //std::cout << "send note channel " << channel << std::endl;
}

static int gestureVote = 0;
void InstrumentModule::update(MetronomeState state)
{

  checkInstrumentChange();
  if (checkStop()) return;
  updateNotesIfNeeded(state);
  sendChordsIfNeeded(state);
  sendMelody(state);
//  updateRhythm(state);
}

void InstrumentModule::sendMelody(MetronomeState state) {
  HandState h = hand();
  
  if (//(state.sixteenth && !state.eighth && rhythm.accentAt(state)) ||
      (h.splay < 0.05 && state.quarter) ||
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
      sendNote(SYNTH_CHANNEL, melodyNote, NOTE_OFF, NULL);
      sendNote(SYNTH_CHANNEL, melodyNote - 12, NOTE_OFF, NULL);
      sendNote(SYNTH_CHANNEL, curSixteenth, velo, &melodyNote);
      sendNote(SYNTH_CHANNEL, curSixteenth - 12, velo, NULL);
      curPos = (curPos + 1) % curSubdiv;
    }
  }

//  vector<bool> out_accents;
//  bool accents[rhythm.numAccents()];
//  rhythm.getAccents(accents);
//  for (int i = 0; i < rhythm.numAccents(); i++) {
//    bool accent = accents[i];
//    if (i == 0 ||
//    (h.splay >= 0.05 && (i % EIGHTH_SUBDIV == 0)) ||
//    (h.splay >= 0.35 && (i % EIGHTH_TRIPLET_SUBDIV == 0)) ||
//	(h.splay >= 0.75 && (i % SIXTEENTH_SUBDIV == 0)))
//      accent = true;
//    out_accents.push_back(accent);
//  }
//  gfx->sendAccents(out_accents);
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

void InstrumentModule::sendChordsIfNeeded(MetronomeState state)
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
