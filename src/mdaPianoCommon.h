#ifndef MDA_PIANO_COMMON_H
#define MDA_PIANO_COMMON_H

#define NPARAMS 12       //number of parameters
#define NOUTS    2       //number of outputs
#define NVOICES 64       //max polyphony
#define SUSTAIN 128
#define SILENCE 0.0001f  //voice choking
#define PARAM_OFFSET 3 //offset for param enum

#ifdef PIANO
#define SAMPLES_PER_NOTE 1
#define NSAMPLES 15
#elif defined EPIANO
#define SAMPLES_PER_NOTE 3
#define NSAMPLES 33
#endif

struct KGRP  //keygroup
{
  long root;  //MIDI root note
  long high;  //highest note
  long loop;
};

typedef struct
{
  long size;     //length of sample data
  short* buffer; //pointer to sample data
} Sample;

static float scale_midi_to_f(unsigned char data)
{
  return 0.0078f * (float)(data);
}

static unsigned char p_offset(unsigned char i)
{
  return (i - PARAM_OFFSET);
}

#endif
