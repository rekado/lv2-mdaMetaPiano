#ifndef MDA_PIANO_VOICE_H
#define MDA_PIANO_VOICE_H

#include "mdaPianoCommon.h"
#include "mdaPiano.peg"

#pragma GCC system_header
#include <lvtk/synth.hpp>

enum Param {
  Default,
  Current
};

class mdaPianoVoice : public lvtk::Voice {
  private:
    float Fs, iFs;

    /// global internal variables
    KGRP  *kgrp;
    Sample *samples;
    uint32_t sample_index;
    short sustain;
    float comb[256];
    float cdep, width, trim;
    float fine, random, stretch;
    float volume, muff, muffvel, sizevel, velsens;
    uint32_t cpos, size;

    // voice state
    uint32_t  delta;  // sample playback
    uint32_t  frac;
    uint32_t  pos;
    uint32_t  end;
    uint32_t  loop;

    float env;  // envelope
    float dec;

    float f0;   // first-order LPF
    float f1;
    float ff;

    float outl;
    float outr;
    // end of voice state
    float default_preset[NPARAMS]; // contains the default preset

  protected:
    unsigned char m_key;

  public:
    mdaPianoVoice(double, Sample*, KGRP*);
    void set_sustain(unsigned short v) { sustain = v; }
    void set_volume(float v) { volume = v; }
    void set_muff(float v) { muff = v; }

    float p_helper(unsigned short, Param);
    void update(Param); // recalculates internal variables
    void on(unsigned char key, unsigned char velocity);
    void release(unsigned char velocity);
    void reset(void);
    bool is_sustained(void) { return (m_key == SUSTAIN); }
    unsigned char get_key(void) const { return m_key; }

    // generates the sound for this voice
    void render(uint32_t, uint32_t);
};

#endif
