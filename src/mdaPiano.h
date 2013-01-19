//See associated .cpp file for copyright and other info

#ifndef MDA_PIANO_H
#define MDA_PIANO_H

#pragma GCC system_header
#include "mdaPianoVoice.h"
#include "mdaPiano.peg"
#include <lv2synth.hpp>


class mdaPiano : public LV2::Synth<mdaPianoVoice, mdaPiano> {
public:
  mdaPiano(double rate);
  ~mdaPiano() {
    for (unsigned char i = 0; i < 15; i++) {
      free(samples[i].buffer);
    }
    free(samples);
  }

  void load_kgrp(KGRP*);
  void load_sample(Sample*, const char*);

  unsigned find_free_voice(unsigned char, unsigned char);
  void handle_midi(uint32_t size, unsigned char* data);
  void setVolume(float);
  void setParameter(unsigned char, float);


private:
  void update();  //my parameter update
  void noteOn(uint32_t note, uint32_t velocity);

  float param[NPARAMS];
  float Fs, iFs;

  ///global internal variables
  KGRP  kgrp[16];
  mdaPianoVoice *voices[NVOICES];
  Sample *samples = (Sample*) malloc (15 * sizeof(Sample));
  uint32_t sustain;
};

#endif
