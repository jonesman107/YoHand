#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include <QObject>
#include <QTimer>
#include <vector>
#include <QKeyEvent>

#include "graphics.h"
#include "metronome.h"

class LeapMux;
class LeapPd;
class Module;

class AudioWidget : public QObject
{
  Q_OBJECT

  int bpm;

private:
  std::vector<Module*> modules;

public:
  AudioWidget(LeapMux *leap, Graphics *gfx,
	      int instr_l, int instr_r, int effect_l, int effect_r,
	      QTimer *timer);

public slots:
  void updateAudio();
  void rapidUpdateAudio();
  void setBPM(int bpm);
  void keyPressEvent(QKeyEvent *keyEvent);

private:
  LeapMux *leap;
  Metronome metronome;
  LeapPd *pd;
  int count;
  Graphics *gfx;
};

#endif
