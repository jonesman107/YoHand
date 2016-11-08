/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <mutex>
#include <vector>

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>

#include "metronome.h"
#include "musictheory.h"

class LeapMux;
struct HandState;

//! [0]
class Graphics
{
public:
  Graphics(LeapMux *leap, int instr_l, int instr_r, int effect_l, int effect_r);

public:
    void setNoteBounds(int min, int max);
    void setSynth(float a, float d, float s, float r);
    void setAttack(float a);
    void setDelay(float d);
    void setSustain(float s);
    void setRelease(float r);

    void setMetronomeState(MetronomeState state);

    // control root note UI element
    void sendRootNote(int root_note, int max);
    void sendKey(int new_key);
    void sendHarmony(Harmony *h);

    // control instrument select UI element
    void sendInstrumentNumber(int instrumentNumber);

    // control BPM display
    void sendBPM(int bpm);

    void sendNote(char note, char velocity);
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);

    void sendAccents(std::vector<bool> accents);

private:
    // hand-painting
    void paintFinger(QPainter *painter, float *fpos, QRect drawrect);
    void paintStretch(QPainter *painter, float *index, float *pinky, QRect drawrect);
    void paintGesture(QPainter *painter, HandState h, QRect drawrect);
    void paintHand(QPainter *painter, HandState h, QRect drawrect);
    void paintLeftHand(QPainter *painter, QRect drawrect);
    void paintRightHand(QPainter *painter, QRect drawrect);

    // general painting
    void paintMetronome(QPainter *painter, MetronomeState st, QRect drawrect);
    void paintKeyboard(QPainter *painter, QRect dst);
    void paintNotes(QPainter *painter, QRect drawrect);
    void paintSynth(QPainter *painter, QRect drawrect);
    void paintKeyAndBpm(QPainter *painter, QRect dst);

    int instr_l, instr_r, effect_l, effect_r;

    std::mutex note_mutex;
    int min_note = Harmony::MIN_NOTE - Harmony::NOTES_PER_OCTAVE;
    int max_note = Harmony::MIN_NOTE + (Harmony::RANGE + 1) * Harmony::NOTES_PER_OCTAVE;
    //int max_note = 127;
    char velocities[128] = { 0 };
    std::list<char> veloQueue;

    std::mutex bpm_mutex;
    int bpm = 80;

    // root note UI attributes
    std::mutex root_note_mutex;
    int root_note = 0;
    int root_note_max = Harmony::NOTES_PER_KEY;
    std::mutex key_mutex;
    int key = 0;
    // root note UI drawing method
    void paintRootNote(QPainter *painter, QRect drawrect);

    // accents UI attributes
    std::mutex accent_mutex;
    std::vector<bool> accents;
    // accents UI drawing method
    void paintAccents(QPainter *painter, QRect dst);

    // instrument select UI attributes
    std::mutex instru_mutex;
    int selected_instrument_number = 0;
    // instrument select UI drawing method
    void paintInstrumentSelect(QPainter *painter, QRect drawrect);

    // LO/HI pass region drawing method
    void paintLoHiPassRegions(QPainter *painter, QRect drawrect);

    // key indicator drawing method
    void paintKeyIndicator(QPainter *painter, QRect dst);

    std::mutex synth_mutex;
    float adsr[4] = { 0 };

    std::mutex metronome_mutex;
    MetronomeState metronome_state;

    QPen circlePen;
    QFont textFont;
    QPen textPen;

    LeapMux *leap;
    Harmony *harmony = NULL;
};
//! [0]

#endif
