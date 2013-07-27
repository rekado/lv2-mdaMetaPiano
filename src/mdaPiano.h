#ifndef MDA_PIANO_H
#define MDA_PIANO_H
//See associated .cpp file for copyright and other info

#include "mdaPianoVoice.h"
#include "ports.peg"

#pragma GCC system_header
#include <lvtk/synth.hpp>
#include <stdio.h>


class mdaPiano : public lvtk::Synth<mdaPianoVoice, mdaPiano> {
public:
  mdaPiano(double);
  ~mdaPiano() {
    for (unsigned char i = 0; i < NSAMPLES; i++) {
      free(samples[i].buffer);
    }
    free(samples);
  }

  bool sustain;
  KGRP kgrp[NSAMPLES];
  mdaPianoVoice *voices[NVOICES];
  Sample *samples = (Sample*) malloc (NSAMPLES * sizeof(Sample));

  void load_kgrp(KGRP*);
  void load_sample(Sample*, const char*);
#ifdef EPIANO
  void tweak_samples(void);
#endif

  unsigned find_free_voice(unsigned char, unsigned char);
  void handle_midi(uint32_t, unsigned char*);
  void setVolume(float);
  void update(void);
};

#endif
