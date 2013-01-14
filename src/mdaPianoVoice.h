#ifndef MDA_PIANO_VOICE_H
#define MDA_PIANO_VOICE_H

#include "mdaPianoCommon.h"
#include "mdaPiano.peg"

#pragma GCC system_header
#include <lv2synth.hpp>

enum Param {
  Default,
  Current
};

class mdaPianoVoice : public LV2::Voice {
  private:
    float Fs, iFs;

    /// global internal variables
    KGRP  *kgrp;
    short *waves;
    float default_preset[NPARAMS]; // contains the default preset
    short sustain;

    // voice state
    uint32_t  delta;  //sample playback
    uint32_t  frac;
    uint32_t  pos;
    uint32_t  end;
    uint32_t  loop;

    float env;  //envelope
    float dec;

    float f0;   //first-order LPF
    float f1;
    float ff;

    float outl;
    float outr;
    uint32_t  note; //remember what note triggered this
    // end of voice state


  public:
    mdaPianoVoice(double, short*, KGRP*);

    float p_helper(unsigned short, Param);
    void update(Param); // recalculates internal variables
    void reset(void);
};

#endif
