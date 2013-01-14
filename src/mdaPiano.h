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

  virtual uint32_t processEvents(VstEvents* events);

  virtual void setParameter(uint32_t index, float value);
  virtual void resume();


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
  uint32_t activevoices, poly, cpos;
  short *waves;
  uint32_t cmax;
  float *comb, cdep, width, trim;
  uint32_t size, sustain;
  float tune, fine, random, stretch;
  float muff, muffvel, sizevel, velsens, volume;
};

#endif
