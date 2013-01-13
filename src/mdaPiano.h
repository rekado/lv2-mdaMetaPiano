//See associated .cpp file for copyright and other info

#ifndef __mdaPiano__
#define __mdaPiano__

#include <string.h>

#include <lv2synth.hpp>

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
  VstInt32  delta;  //sample playback
  VstInt32  frac;
  VstInt32  pos;
  VstInt32  end;
  VstInt32  loop;
  
  float env;  //envelope
  float dec;

  float f0;   //first-order LPF
  float f1;
  float ff;

  float outl;
  float outr;
  VstInt32  note; //remember what note triggered this
};


struct KGRP  //keygroup
{
  VstInt32  root;  //MIDI root note
  VstInt32  high;  //highest note
  VstInt32  pos;
  VstInt32  end;
  VstInt32  loop;
};

class mdaPiano : public LV2::Synth<mdaPianoVoice, mdaPiano> {
public:
  mdaPiano(double rate);
  ~mdaPiano();

  virtual void process(float **inputs, float **outputs, VstInt32 sampleframes);
  virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleframes);
  virtual VstInt32 processEvents(VstEvents* events);

  virtual void setParameter(VstInt32 index, float value);
  virtual void resume();


private:
  void update();  //my parameter update
  void noteOn(VstInt32 note, VstInt32 velocity);

  float param[NPARAMS];
  float Fs, iFs;

  #define EVENTBUFFER 120
  #define EVENTS_DONE 99999999
  VstInt32 notes[EVENTBUFFER + 8];  //list of delta|note|velocity for current block

  ///global internal variables
  KGRP  kgrp[16];
  VOICE voice[NVOICES];
  VstInt32  activevoices, poly, cpos;
  short *waves;
  VstInt32  cmax;
  float *comb, cdep, width, trim;
  VstInt32  size, sustain;
  float tune, fine, random, stretch;
  float muff, muffvel, sizevel, velsens, volume;
};

#endif
