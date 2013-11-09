#ifndef MDA_PIANO_VOICE_H
#define MDA_PIANO_VOICE_H

#include "mdaPianoCommon.h"
#include "ports.peg"

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
    bool dropped;
    float width;
    float fine, random;
    float sizevel, velsens, volume;
    long size;

#ifdef PIANO
    float comb[256];
    float cdep, trim;
    float stretch;
    float muff, muffvel;
    uint32_t cpos;
#endif
#ifdef EPIANO
    float overdrive;
    float lfo0, lfo1, dlfo, lmod, rmod;
    float treb, tfrq, tl, tr;
#endif

// TODO: uint32_t or long?
#ifdef PIANO
    // voice state
    uint32_t  delta;  // sample playback
    uint32_t  frac;
    uint32_t  pos;
    uint32_t  end;
    uint32_t  loop;
#elif defined EPIANO
    long  delta;  // sample playback
    long  frac;
    long  pos;
    long  end;
    long  loop;
#endif

    float env;  // envelope
    float dec;

#ifdef PIANO
    float f0;   // first-order LPF
    float f1;
    float ff;
#endif

    float outl;
    float outr;
    // end of voice state

    float default_preset[NPARAMS]; // contains the default preset

  protected:
    unsigned char m_key;
    unsigned char down_note;

  public:
    mdaPianoVoice(double, Sample*, KGRP*);
    void set_sustain(unsigned short v) { sustain = v; }
    void set_volume(float v) { volume = v; }
#ifdef PIANO
    void set_muff(float v) { muff = v; }
#endif

    float p_helper(unsigned short, Param);
    void update(Param); // recalculates internal variables
    void on(unsigned char key, unsigned char velocity);
    void drop(void);
    void release(unsigned char velocity);
    void reset(void);
    bool is_sustained(void) { return (m_key == SUSTAIN); }
    bool was_dropped(void) {return dropped; }
    unsigned char get_key(void) const { return m_key; }
    unsigned char get_note(void) const {return down_note; }
    float get_env(void) { return env; }

    // generates the sound for this voice
    void render(uint32_t, uint32_t);
};

#endif
