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

#include <iostream>

#include "glwidget.h"
#include "widget.h"
#include "window.h"
#include "leapcontroller.h"
#include "leapmux.h"

#include <QGridLayout>
#include <QLabel>
#include <QTimer>

#define AUDIO_UPDATE_TICKS 190

//! [0]
Window::Window()
{
    setWindowTitle(tr("YoHand Sebastian Bach"));
    setMinimumSize(1240, 800);

    // General init
    LeapController *controller = new LeapController();
    LeapMux *mux = new LeapMux(controller, 4);

    int instr_l = mux->newChannel(LEFT_HAND);
    int instr_r = mux->newChannel(RIGHT_HAND);
    int effect_l = mux->newChannel(LEFT_HAND);
    int effect_r = mux->newChannel(RIGHT_HAND);
    
    graphics = new Graphics(mux, instr_l, instr_r, effect_l, effect_r);

    // Initialize the control modules.
    GLWidget *openGL = new GLWidget(graphics, this);
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    audio = new AudioWidget(mux, graphics, instr_l, instr_r,
					 effect_l, effect_r, timer);

    // Build our UI.
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(openGL, 0, 1);
    setLayout(layout);

    // Set up all timers needed to keep the system running.
    
    // Graphics timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), openGL, SLOT(animate()));
    timer->start(50);

    // Audio timer that fires as fast as possible
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), audio, SLOT(rapidUpdateAudio()));
    timer->start(10);
}
//! [0]

void Window::keyPressEvent(QKeyEvent *keyEvent)
{
  audio->keyPressEvent(keyEvent);
}
