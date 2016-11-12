#include <iostream>

#include "audiowidget.h"
#include "leapmux.h"
#include "leappd.h"
#include "module.h"
#include "mellotronmodule.h"
#include "synthmodule.h"
#include "effectsmodule.h"

#define BPM 80

AudioWidget::AudioWidget(LeapMux *leap, Graphics *gfx,
                         int instr_l, int instr_r,
                         int effect_l, int effect_r,
                         QTimer *timer)
    : leap(leap), metronome(timer, BPM)
{
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAudio()));
    pd = new LeapPd("polysynth_and_effects.pd");

    modules.push_back(new MellotronModule(leap, pd, gfx, instr_l));
    modules.push_back(new SynthModule(leap, pd, gfx, instr_r));

    count = 0;
    this->gfx = gfx;
}

void AudioWidget::updateAudio()
{
    metronome.tick();
    MetronomeState st = metronome.state();
    gfx->setMetronomeState(st);
    for (auto module : modules) {
        module->update(st);
    }
}

void AudioWidget::rapidUpdateAudio() {
    for (auto module : modules) {
        module->rapidUpdate(10);
    }
}

void AudioWidget::setBPM(int bpm)
{
    metronome.setBPM(bpm);
}

void AudioWidget::keyPressEvent(QKeyEvent *keyEvent) {
    int keyCode = keyEvent->key();
    // if keyboard key is 0-9 on the top row
    if ((Qt::Key_1 <= keyCode && keyCode <= Qt::Key_9) ||
            keyCode == Qt::Key_0 ||
            keyCode == Qt::Key_Minus ||
            keyCode == Qt::Key_Equal) {
        int instrumentNumber, i;
        if (Qt::Key_1 <= keyCode && keyCode <= Qt::Key_9) {
            i = 0;
            instrumentNumber = keyCode - Qt::Key_1;
        } else if (keyCode == Qt::Key_0) {
            i = 0;
            instrumentNumber = 9;
        } else if (keyCode == Qt::Key_Minus) {
            i = 1;
            instrumentNumber = 10;
        } else if (keyCode == Qt::Key_Equal) {
            i = 1;
            instrumentNumber = 11;
        }
        modules.at(i)->changeInstrument(instrumentNumber);
        // send instrument change to all modules
//        for (auto module : modules) {
//            module->changeInstrument(instrumentNumber);
//        }
        // update graphics
        gfx->sendInstrumentNumber(instrumentNumber);
    } else if (keyCode == Qt::Key_Right) {
        for (auto module : modules) {
            module->incrementKey();
        }
    } else if (keyCode == Qt::Key_Left) {
        for (auto module : modules) {
            module->decrementKey();
        }
    } else if (keyCode == Qt::Key_Up) {
        metronome.setBPM(std::min(metronome.getBPM() + 10, 200));
        gfx->sendBPM(metronome.getBPM());
    } else if (keyCode == Qt::Key_Down) {
        metronome.setBPM(std::max(metronome.getBPM() - 10, 50));
        gfx->sendBPM(metronome.getBPM());
    }

}
