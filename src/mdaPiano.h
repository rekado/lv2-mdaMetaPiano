//See associated .cpp file for copyright and other info

#ifndef __mdaPiano__
#define __mdaPiano__

#pragma GCC system_header
#include <lv2synth.hpp>
#include <string.h>

#define NPARAMS 12       //number of parameters
#define NPROGS   8       //number of programs
#define NOUTS    2       //number of outputs
#define NVOICES 32       //max polyphony
#define SUSTAIN 128
#define SILENCE 0.0001f  //voice choking
#define WAVELEN 586348   //wave data bytes

class mdaPianoProgram
{
  friend class mdaPiano;
public:
  mdaPianoProgram();
  ~mdaPianoProgram() {}

private:
  float param[NPARAMS];
  char  name[24];
};


struct VOICE  //voice state
{
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
};


struct KGRP  //keygroup
{
  uint32_t  root;  //MIDI root note
  uint32_t  high;  //highest note
  uint32_t  pos;
  uint32_t  end;
  uint32_t  loop;
};

class mdaPiano : public LV2::Synth<mdaPianoVoice, mdaPiano> {
public:
  mdaPiano(double rate);
  ~mdaPiano();

  virtual void process(float **inputs, float **outputs, uint32_t sampleframes);
  virtual void processReplacing(float **inputs, float **outputs, uint32_t sampleframes);
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
  VOICE voice[NVOICES];
  uint32_t activevoices, poly, cpos;
  short *waves;
  uint32_t cmax;
  float *comb, cdep, width, trim;
  uint32_t size, sustain;
  float tune, fine, random, stretch;
  float muff, muffvel, sizevel, velsens, volume;
};

#endif
