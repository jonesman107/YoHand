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

#include <mutex>
#include <iostream>
#include <vector>

#include "graphics.h"
#include "leapmux.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QFont>

static QColor BACKGROUND_COLOR = Qt::black;

Graphics::Graphics(LeapMux *leap, int instr_l, int instr_r, int effect_l, int effect_r) :
    instr_l(instr_l), instr_r(instr_r), effect_l(effect_l), effect_r(effect_r)
{
    this->leap = leap;

    circlePen = QPen(Qt::white);
    circlePen.setWidth(3);

    textPen = QPen(Qt::white);
    textFont.setPixelSize(16);
}

void Graphics::setNoteBounds(int min, int max) {
    lock_guard<mutex> lock(note_mutex);
    min_note = min; max_note = max;
}

void Graphics::sendNote(char note, char velocity)
{
    lock_guard<mutex> lock(note_mutex);
    veloQueue.push_back(note);
    veloQueue.push_back(velocity);
}

// control root note UI element
void Graphics::sendRootNoteLeft(int note, int max) {
    lock_guard<mutex> lock(root_note_left_mutex);
    root_note_left = note;
    root_note_max = max;
}

void Graphics::sendRootNoteRight(int note) {
    lock_guard<mutex> lock(root_note_right_mutex);
    std::cout << "send root note right: " << note << std::endl;
    root_note_right = note;
}

void Graphics::sendHarmony(Harmony *new_harmony) {
    harmony = new_harmony;
}

void Graphics::sendKey(int new_key) {
    lock_guard<mutex> lock(key_mutex);
    key = new_key;
}

void Graphics::setSynth(float a, float d, float s, float r)
{
    lock_guard<mutex> lock(synth_mutex);
    adsr[0] = a, adsr[1] = d, adsr[2] = s, adsr[3] = r;
}

void Graphics::setAttack(float a)
{
    lock_guard<mutex> lock(synth_mutex);
    adsr[0] = a;
}

void Graphics::setDelay(float d)
{
    lock_guard<mutex> lock(synth_mutex);
    adsr[1] = d;
}

void Graphics::setSustain(float s)
{
    lock_guard<mutex> lock(synth_mutex);
    adsr[2] = s;
}

void Graphics::setRelease(float r)
{
    lock_guard<mutex> lock(synth_mutex);
    adsr[3] = r;
}

void Graphics::setMetronomeState(MetronomeState state)
{
    lock_guard<mutex> lock(metronome_mutex);
    metronome_state = state;
}

void Graphics::sendInstrumentNumber(int instrument_number) {
    lock_guard<mutex> lock(instru_mutex);
    if (instrument_number < 2)
        selected_instrument_left = instrument_number;
    else
        selected_instrument_right = instrument_number;
}

void Graphics::sendAccents(std::vector<bool> a) {
    lock_guard<mutex> lock(accent_mutex);
    accents = a;
}

void Graphics::sendBPM(int b) {
    lock_guard<mutex> lock(bpm_mutex);
    bpm = b;
}

void Graphics::paintNotes(QPainter *painter, QRect drawrect)
{
    while (!veloQueue.empty()) {
        lock_guard<mutex> lock(note_mutex);
        char note = veloQueue.front();
        veloQueue.pop_front();
        char velo = veloQueue.front();
        veloQueue.pop_front();
        velocities[note] = velo;
    }

    int height = drawrect.height(), width = drawrect.width();

    float note_width = (float)height / (max_note - min_note);
    painter->setPen(QPen(Qt::white, 2));
    for (int i = min_note; i < max_note; i++) {
        painter->setBrush(QColor(0, 2 * velocities[i], 0));
        painter->drawRect(0, note_width * (max_note - i - 1), width, note_width);
    }
}

void Graphics::paintSynth(QPainter *painter, QRect drawrect)
{
    lock_guard<mutex> lock(synth_mutex);
    int width = drawrect.width(), height = drawrect.height();
    float tw = (adsr[0] + adsr[1] + adsr[3]) * 2;
    float res = width / tw;

    float ax = res * adsr[0];
    float dx = ax + res * adsr[1];
    float sx = dx + res * (tw / 2);
    float rx = sx + res * adsr[3];
    float sy = height * (1.0 - adsr[2]);

    painter->setBrush(Qt::black);
    painter->drawRect(drawrect);
    painter->drawLine(0, height, ax, 0);
    painter->drawLine(ax, 0, dx, sy);
    painter->drawLine(dx, sy, sx, sy);
    painter->drawLine(sx, sy, rx, height);
}

void Graphics::paintFinger(QPainter *painter, float *fpos, QRect drawrect)
{
    int width = drawrect.width(), height = drawrect.height();

    if (fpos[0] == 0.0 || fpos[1] == 0.0
            || fpos[0] == 1.0 || fpos[1] == 1.0) return;

    float x = fpos[0] * width;
    float y = height - (fpos[1] * height);
    float r = (height / 12) / (fpos[2] + 1.0);

    painter->drawEllipse(x - r/2, y - r/2, r, r);
}

void Graphics::paintStretch(QPainter *painter, float *index, float *pinky, QRect drawrect)
{
    int width = drawrect.width(), height = drawrect.height();

    float index_x = index[0] * width;
    float index_y = height - (index[1] * height);
    float index_h = (height / 60) / (index[2] + 0.5);

    float pinky_x = pinky[0] * width;
    float pinky_y = height - (pinky[1] * height);
    float pinky_h = (height / 60) / (pinky[2] + 0.5);

    QLineF topLine (index_x, index_y, pinky_x, pinky_y);

    QLineF indexLine;
    indexLine.setP1(QPointF(index_x, index_y));
    indexLine.setLength(index_h);
    indexLine.setAngle(topLine.angle() - 90.0);

    QLineF pinkyLine;
    pinkyLine.setP1(QPointF(pinky_x, pinky_y));
    pinkyLine.setLength(pinky_h);
    pinkyLine.setAngle(topLine.angle() - 90);

    QPointF pts[4] = {
        topLine.p1(),
        topLine.p2(),
        pinkyLine.p2(),
        indexLine.p2()
    };

    painter->drawPolygon(pts, 4);
}

void Graphics::paintGesture(QPainter *painter, HandState h, QRect drawrect)
{
    if (h.x == 0 && h.y == 0 && h.z == 0) return;

    // sort fingers by z-value then paint them all
    vector<float *> sorted;
    // hardcoded 5 fingers
    for (int i = 0; i < 5; i++) {
        sorted.push_back(h.fpos[i]);
    }
    sort (sorted.begin(), sorted.end(), [](float *a, float * b) {
        return a[2] > b[2];
    });
    for (float *fgr : sorted) paintFinger (painter, fgr, drawrect);
    // */
}

void Graphics::paintHand(QPainter *painter, HandState h, QRect drawrect)
{
    if (h.x == 0 && h.y == 0 && h.z == 0) return;

    painter->setClipRegion(drawrect);
    painter->setClipping(true);

    int width = drawrect.width(), height = drawrect.height();

    float x = h.x * width;
    float y = height - (h.y * height);
    float r = (height / 12) / (h.z + 1.0);

    int theta = atan2 (h.nx, -h.ny) * 180.0 / 3.1415;
    painter->save();
    painter->setPen(Qt::gray);
    painter->drawLine(x, 0, x, height);
    painter->drawLine(0, y, width, y);

    painter->restore();
    painter->drawChord(x - r, y - r, r*2, r*2, theta * 16, 180 * 16);

    paintGesture (painter, h, drawrect);
    painter->setClipping(false);
}

void Graphics::paintMetronome(QPainter *painter, MetronomeState st, QRect drawrect)
{
    lock_guard<mutex> lock(metronome_mutex);
    painter->setClipRegion(drawrect);
    painter->setClipping(true);

    painter->setPen(Qt::black);

    painter->setBrush(Qt::blue);
    painter->drawChord(drawrect.x(), drawrect.y() + drawrect.height() / 2,
                       100, 100, 2 * 360 * st.sixteenthTripletNum / 3, 180 * 16);
    painter->setBrush(Qt::green);
    painter->drawChord(drawrect.x(), drawrect.y() + drawrect.height() / 2,
                       100, 100, 2 * 360 * st.sixteenthTripletNum / 3 + 180 * 16, 180 * 16);

    painter->setBrush(Qt::white);
    if (!st.sixteenthNum)
        painter->drawEllipse(drawrect.x(), drawrect.y() + drawrect.height() / 2, 100, 100);
    else
        painter->drawEllipse(drawrect.x() + 40, drawrect.y() + drawrect.height() / 2 + 40, 20, 20);

    painter->setClipping(false);
}

// current key indicator drawing method
void Graphics::paintKeyIndicator(QPainter *painter, QRect dst) {
    lock_guard<mutex> lock(key_mutex);
    painter->setPen(textPen);
    painter->setFont(textFont);
    char buffer[30];
    sprintf(buffer, "Key: %s ", harmony->getKeyName(key).c_str());
    painter->drawText(dst, Qt::AlignRight | Qt::AlignTop, QString(buffer));
}

// LO/HI pass region drawing method
void Graphics::paintLoHiPassRegions(QPainter *painter, QRect dst) {
    // hi
    QRect hi = dst;
    hi.setHeight(dst.height() * 0.4);
    // lo
    QRect lo = hi;
    lo.translate(0, dst.height() * 0.6);
    // paint hi
    QLinearGradient hi_gradient(hi.topLeft(), hi.bottomLeft());
    hi_gradient.setColorAt(0, QColor(197, 202, 233));
    hi_gradient.setColorAt(1, Qt::black);
    painter->fillRect(hi, hi_gradient);
    // paint lo
    QLinearGradient lo_gradient(lo.topLeft(), lo.bottomLeft());
    lo_gradient.setColorAt(0, Qt::black);
    lo_gradient.setColorAt(1, QColor(24, 32, 120));
    painter->fillRect(lo, lo_gradient);
}

// instrument select drawing method
static QColor ISEL_SELECTED = QColor(92, 107, 192);
static QColor ISEL_NOT_SELECTED = QColor(26, 35, 126);
static int ISEL_PAD = 3;
static int NUM_INSTRUMENTS = 4;

//static char* ISEL_NAMES[] = {"Flute", "M300A", "M300B", "Violin", "Brass", "Cello", "Strings", "Choir", "Wood-\nwind", "GC3\nBrass", "Square\nWave", "Sine\nWave"};
static char* ISEL_NAMES[] = {"LH Saw", "LH Sine", "RH Saw", "RH Sine"};

void Graphics::paintInstrumentSelect(QPainter *painter, QRect dst) {
    lock_guard<mutex> lock(instru_mutex);
    for (int i = 0; i < NUM_INSTRUMENTS; i++) {
        int pos = (int) ((float) i * (float) dst.width() / (float) NUM_INSTRUMENTS);
        int width = (int) ((float) (i + 1) * (float) dst.width() / (float) NUM_INSTRUMENTS) - pos;
        if (i == 0) {
            pos += ISEL_PAD;
            width -= ISEL_PAD;
        }
        // draw selector
        QRect selectorRect = QRect(dst.x() + pos, dst.y(), width - ISEL_PAD, dst.height() - ISEL_PAD);
        QColor color = (selected_instrument_left == i || selected_instrument_right == i) ? ISEL_SELECTED : ISEL_NOT_SELECTED;
        painter->fillRect(selectorRect, color);
        // draw text
        painter->setPen(textPen);
        painter->setFont(textFont);
        // draw selector number
        selectorRect.setHeight(selectorRect.height() / 2);
        QString keyString;
        if (0 <= i && i <= 8) {
            keyString = QString::number(i + 1);
        } else {
            switch (i) {
            case 9:
                keyString = QStringLiteral("0");
                break;
            case 10:
                keyString = QStringLiteral("-");
                break;
            default: // case 11:
                keyString = QStringLiteral("=");
                break;
            }
        }
        painter->drawText(selectorRect, Qt::AlignCenter, keyString);
        // draw selector name
        selectorRect.translate(0, selectorRect.height());
        painter->drawText(selectorRect, Qt::AlignCenter, QString(ISEL_NAMES[i]));
    }
}

// root note drawing method
static QColor ROOT_PLAYING = Qt::white;
static QColor ROOT_INVALID = Qt::black;
static QColor ROOT_KEY = QColor(50, 80, 50);
static QColor ROOT_MAJOR = QColor(50, 50, 80);
static QColor ROOT_DEFAULT = QColor(50, 50, 50);
static int ROOT_PADDING = 3;

void Graphics::paintRootNoteLeft(QPainter *painter, QRect dst) {
    lock_guard<mutex> lock(root_note_left_mutex);
    paintRootNote(painter, dst, root_note_left);
}

void Graphics::paintRootNoteRight(QPainter *painter, QRect dst) {
    lock_guard<mutex> lock(root_note_right_mutex);
    paintRootNote(painter, dst, root_note_right);
}

void Graphics::paintRootNote(QPainter *painter, QRect dst, int root_note) {
    // root notes
    for (int i = 0; i < root_note_max; i++) {
        int y = dst.y() + dst.height() - i * dst.height() / root_note_max;
        int height = dst.height() / root_note_max - ROOT_PADDING;
        QColor color = ROOT_DEFAULT;
        if (harmony) {
            int root_idx = (7 + i - harmony->root_note_index[key]) % 7;
            if (root_idx == 0) color = ROOT_KEY;
            else if (root_idx == 6) color = ROOT_INVALID;
            else if (!harmony->is_minor[root_idx]) color = ROOT_MAJOR;
        }
        if (i == root_note) color = color.lighter(300);
        painter->fillRect(QRect(dst.x(), y - height, dst.width(), height), color);
    }
}

// accents UI element drawing method
static QColor ACCENT = QColor(63, 81, 181);
static int ACCENT_PADDING = 5;
static QBrush METRONOME_PTR = QBrush(QColor(255, 255, 255));
static int METRONOME_PTR_WIDTH = 30;
static int METRONOME_PTR_HEIGHT = 40;

void Graphics::paintAccents(QPainter *painter, QRect dst) {
    lock_guard<mutex> lock(accent_mutex);
    // draw accents
    int i = 0;
    int len = accents.size();
    for (auto it = accents.begin(); it != accents.end(); it++) {
        // only check every third boolean in accents vector
        if (i % 3 == 0 && *it) {
            int pos = (int) ((float) i / (float) len * (float) dst.width());
            int width = (int) ((float) (i + 3) / (float) len * (float) dst.width()) - pos;
            if (i == 0) {
                pos += ACCENT_PADDING;
                width -= ACCENT_PADDING;
            }
            QRect rect = QRect(dst.x() + pos, dst.y() + ACCENT_PADDING, width - ACCENT_PADDING, dst.height() - 2*ACCENT_PADDING);
            painter->fillRect(rect, ACCENT);
        }
        i++;
    }
    // draw metronome pointer
    float time = (float) metronome_state.sixteenthTripletNum / 47.0 ;
    int ptrX = dst.x() + time * dst.width();
    int ptrY = dst.y();
    QPointF ptrPoints[3] = {
        QPointF(ptrX - METRONOME_PTR_WIDTH / 2, ptrY),
        QPointF(ptrX + METRONOME_PTR_WIDTH / 2, ptrY),
        QPointF(ptrX, ptrY + METRONOME_PTR_HEIGHT)
    };
    painter->setBrush(METRONOME_PTR);
    painter->setPen(Qt::NoPen);
    painter->drawConvexPolygon(ptrPoints, 3);
}

void Graphics::paintLeftHand(QPainter *painter, QRect dst) {
    painter->setPen(circlePen);
    painter->save();
    if (leap->isFrozen(effect_l)) {
        painter->setBrush(QColor(128, 128, 128));
        paintHand (painter, leap->currentGesture(effect_l), dst);
        auto pen = painter->pen();
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    painter->setBrush(QColor(0, 128, 0));
    paintHand (painter, leap->currentGesture(effect_l, true), dst);
    painter->restore();
}

void Graphics::paintRightHand(QPainter *painter, QRect dst) {
    painter->setPen(circlePen);
    painter->save();
    painter->translate(dst.width(), 0);
    if (leap->isFrozen(instr_r)) {
        painter->setBrush(QColor(128, 128, 128));
        paintHand (painter, leap->currentGesture(instr_r), dst);
        auto pen = painter->pen();
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    painter->setBrush(QColor(128, 0, 0));
    paintHand (painter, leap->currentGesture(instr_r, true), dst);
    painter->restore();
}

// paint the little box with key & BPM info
void Graphics::paintKeyAndBpm(QPainter *painter, QRect dst) {
    lock_guard<mutex> keyLock(key_mutex);
    lock_guard<mutex> bpmLock(bpm_mutex);
    // paint border
    painter->fillRect(QRect(dst.x() - 2, dst.y() - 2, dst.width() + 4, dst.height() + 4), Qt::white);
    // paint background
    painter->fillRect(dst, BACKGROUND_COLOR);
    // paint text
    painter->setPen(textPen);
    painter->setFont(textFont);
    char buffer[30];
    // paint key on left side
    QRect left_rect = dst;
    left_rect.setWidth(dst.width() / 2);
    sprintf(buffer, "Key\n%s", harmony->getKeyName(key).c_str());
    painter->drawText(left_rect, Qt::AlignCenter, QString(buffer));
    // paint BPM on right side
    QRect right_rect = dst;
    right_rect.setWidth(dst.width() - left_rect.width());
    right_rect.translate(left_rect.width(), 0);
    sprintf(buffer, "BPM\n%d", bpm); // TODO
    painter->drawText(right_rect, Qt::AlignCenter, QString(buffer));
}

static int INSTRU_SELECT_HEIGHT = 75;
static int ACCENTS_HEIGHT = 75;
static int KEY_AND_BPM_WIDTH = 200;
static int KEY_AND_BPM_HEIGHT = 50;

void Graphics::paint(QPainter *painter, QPaintEvent *event, int elapsed)
{
    // get the size of the window
    QRect window_rect = event->rect();

    // fill window with background
    painter->fillRect(window_rect, BACKGROUND_COLOR);

    // instrument selector rectangle
    QRect instru_select_rect = window_rect;
    instru_select_rect.setHeight(INSTRU_SELECT_HEIGHT);
    instru_select_rect.translate(0, window_rect.height() - instru_select_rect.height());

    // accents rectangle
    QRect accents_rect = window_rect;
    accents_rect.setHeight(ACCENTS_HEIGHT);
    accents_rect.translate(0, window_rect.height() - instru_select_rect.height() - accents_rect.height());

    // left side
    QRect left_rect = window_rect;
    left_rect.setWidth(window_rect.width() / 2);
    left_rect.setHeight(window_rect.height() - instru_select_rect.height() - accents_rect.height());

    // right side
    QRect right_rect = window_rect;
    right_rect.setWidth(window_rect.width() - left_rect.width());
    right_rect.setHeight(window_rect.height() - instru_select_rect.height() - accents_rect.height());
    right_rect.translate(left_rect.width(), 0);

    // paint instrument select UI element
    paintInstrumentSelect(painter, instru_select_rect);

    // paint accents UI element
    paintAccents(painter, accents_rect);

    // paint root note UI element
    paintRootNoteLeft(painter, left_rect);
    paintRootNoteRight(painter, right_rect);

    // paint keyboard
    //  QRect keyboard_rect = left_rect;
    //  keyboard_rect.setWidth(left_rect.width() / 8);
    //  paintNotes(painter, keyboard_rect);

    //  QRect keyboard_rect = right_rect;
    //  keyboard_rect.setWidth(right_rect.width() / 8);
    //  paintNotes(painter, keyboard_rect);

    // paint left hand
    paintLeftHand(painter, left_rect);

    // draw HI/LO pass regions
    //  paintLoHiPassRegions(painter, right_rect);

    // paint right hand
    paintRightHand(painter, left_rect);

    // paint divider
    painter->setPen(Qt::black);
    painter->drawLine(right_rect.x(), right_rect.y(), right_rect.x(), right_rect.height());

    // paint key and BPM
    QRect key_bpm_rect = QRect(window_rect.x() + window_rect.width() / 2 - KEY_AND_BPM_WIDTH / 2,
                               window_rect.y(),
                               KEY_AND_BPM_WIDTH,
                               KEY_AND_BPM_HEIGHT);
    paintKeyAndBpm(painter, key_bpm_rect);
}
