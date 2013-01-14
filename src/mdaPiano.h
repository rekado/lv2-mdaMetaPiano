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
    free(waves);
  }

  void load_kgrp(KGRP*);
  void load_samples(short**);

  void handle_midi(uint32_t size, unsigned char* data);
  void setVolume(float);
  void setParameter(unsigned char, float);


private:
  void update();  //my parameter update
  void noteOn(uint32_t note, uint32_t velocity);

  float param[NPARAMS];
  float Fs, iFs;

  #define EVENTBUFFER 120
  #define EVENTS_DONE 99999999
  uint32_t notes[EVENTBUFFER + 8];  //list of delta|note|velocity for current block

  ///global internal variables
  KGRP  kgrp[16];
  mdaPianoVoice *voices[NVOICES];
  uint32_t activevoices;
  short *waves;
  uint32_t cmax;
  uint32_t sustain;
};

#endif
