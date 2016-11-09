#include "effectsmodule.h"
#include <iostream>

EffectsModule::EffectsModule(LeapMux *mux, LeapPd *pd, Graphics *gfx, int channel) :
  Module(mux, pd, channel), gfx(gfx)
{
    //pd->sendFloat("bs_sound_type", 1);
    //std::cout << "clarinet sound" << std::endl;

  pd->sendFloat("bs_wahwah_on", 1);
  pd->sendFloat("bs_reverb_on", 1);
  pd->sendFloat("bs_rev_wet_dry", 0.5);
  pd->sendFloat("bs_liveness", 80.0);
  pd->sendFloat("bs_damping", 75.0);
  pd->sendFloat("bs_crossover", 1200.0);
}

float clamp(float v, float min, float max)
{
  if (v < min) return min;
  if (v > max) return max;
  return v;
}

#define HIPASS_THRESH 0.6
#define LOPASS_THRESH 0.4

bool EffectsModule::update(MetronomeState state)
{
  HandState hg = hand();
  if (hg.x == 0 && hg.y == 0 && hg.z == 0) return false;
  if (hg.numExtended == 0) return false;

  float scale = clamp((hg.x - 0.4) * 4, 0, 1);
  if (hg.y > HIPASS_THRESH) {
      pd->sendFloat("bs_filter_type", 1);
      float cutoff = (float) std::min(100, (int) (127 * (hg.y - HIPASS_THRESH) / (1.0 - HIPASS_THRESH)));
      pd->sendFloat("bs_filter_cutoff", cutoff * scale);
  } else if (hg.y < LOPASS_THRESH) {
      pd->sendFloat("bs_filter_type", 3);
      float cutoff = (float) std::max(35, (int) (127 * hg.y / LOPASS_THRESH));
      pd->sendFloat("bs_filter_cutoff", cutoff * scale + 127.0 * (1 - scale));
  } else {
      pd->sendFloat("bs_filter_type", 0);
  }

  pd->sendFloat("bs_voice_filt_on", 1);
  pd->sendFloat("bs_start", 20000);
  pd->sendFloat("bs_end", 20000);
  pd->sendFloat("bs_time", 200);
  pd->sendFloat("bs_q", 30);
  pd->sendFloat("bs_volume", 0.5);
  if (scale != 1) {
    pd->sendFloat("bs_start", 20000 - 10000 * (1.0 - scale) * (1.0 - hg.y) * (1.0 - hg.y));
    pd->sendFloat("bs_end", 20000 - 10000 * (1.0 - scale) * (2.0 - hg.y));
    pd->sendFloat("bs_time", 100 + 100 * hg.y);
  }

  if (hg.nx < -.2) {
    float wahwah_amt = (-hg.nx - .2) / .8;
    
    pd->sendFloat("bs_wahwah_lfo_freq", wahwah_amt * 5);
    pd->sendFloat("bs_wahwah_max_freq", 6000);
    pd->sendFloat("bs_wahwah_min_freq", 3000);
    pd->sendFloat("bs_wahwah_q", wahwah_amt * wahwah_amt * 5);
  }

  float reverb = clamp((-hg.nz - 0.3) / 0.7, 0, 1);
  pd->sendFloat("bs_reverb_on", 1);
  pd->sendFloat("bs_rev_wet_dry", 1.0 + reverb);//reverb * 2.0);
  pd->sendFloat("bs_liveness", 90);
  pd->sendFloat("bs_damping", 80);
  pd->sendFloat("bs_crossover", 2500);

  return true;
}
