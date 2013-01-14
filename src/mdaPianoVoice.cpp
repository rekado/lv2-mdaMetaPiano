#include "mdaPianoVoice.h"

mdaPianoVoice::mdaPianoVoice(double rate, short * samples, KGRP * master_kgrp) {
  //set tuning
  Fs = rate;
  iFs = 1.0f/Fs;

  waves = samples;
  kgrp  = master_kgrp;

  default_preset[p_offset(p_envelope_decay)]       = 0.500f;
  default_preset[p_offset(p_envelope_release)]     = 0.500f;
  default_preset[p_offset(p_hardness_offset)]      = 0.500f;
  default_preset[p_offset(p_velocity_to_hardness)] = 0.500f;
  default_preset[p_offset(p_muffling_filter)]      = 0.803f;
  default_preset[p_offset(p_velocity_to_muffling)] = 0.251f;
  default_preset[p_offset(p_velocity_sensitivity)] = 0.376f;
  default_preset[p_offset(p_stereo_width)]         = 0.500f;
  default_preset[p_offset(p_polyphony)]            = 0.330f;
  default_preset[p_offset(p_fine_tuning)]          = 0.500f;
  default_preset[p_offset(p_random_detuning)]      = 0.246f;
  default_preset[p_offset(p_stretch_tuning)]       = 0.500f;

  comb = new float[256];
  reset();
}


void mdaPianoVoice::reset() {
  env = 0.0f;
  dec = 0.99f;
  muff = 160.0f;
  volume = 0.2f;
  sustain = 0;
  cpos = 0;
  m_key = LV2::INVALID_KEY;
}


float mdaPianoVoice::p_helper(unsigned short id, Param d) {
  if (d == Default)
    return default_preset[p_offset(id)];
  else
    return *p(id);
}


void mdaPianoVoice::on(unsigned char note, unsigned char velocity)
{
  // store key that turned this voice on (used in 'get_key')
  m_key = key;

  // TODO: replace with this voice's local copy
  float * param = programs[curProgram].param;
  float l=99.0f;
  uint32_t  v, vl=0, k, s;

  if(velocity>0)
  {
    // TODO: move this to mdaPiano.cpp
    /*
    if(activevoices < poly) //add a note
    {
      vl = activevoices;
      activevoices++;
    }
    else //steal a note
    {
      for(v=0; v<poly; v++)  //find quietest voice
      {
        if(voice[v].env < l) { l = voice[v].env;  vl = v; }
      }
    }
    */

    k = (note - 60) * (note - 60);
    l = fine + random * ((float)(k % 13) - 6.5f);  //random & fine tune
    if(note > 60) l += stretch * (float)k; //stretch

    s = size;
    if(velocity > 40) s += (uint32_t)(sizevel * (float)(velocity - 40));

    k = 0;
    while(note > (kgrp[k].high + s)) k++;  //find keygroup

    l += (float)(note - kgrp[k].root); //pitch
    l = 22050.0f * iFs * (float)exp(0.05776226505 * l);
    voice[vl].delta = (uint32_t)(65536.0f * l);
    voice[vl].frac = 0;
    voice[vl].pos = kgrp[k].pos;
    voice[vl].end = kgrp[k].end;
    voice[vl].loop = kgrp[k].loop;

    voice[vl].env = (0.5f + velsens) * (float)pow(0.0078f * velocity, velsens); //velocity

    l = 50.0f + param[4] * param[4] * muff + muffvel * (float)(velocity - 64); //muffle
    if(l < (55.0f + 0.25f * (float)note)) l = 55.0f + 0.25f * (float)note;
    if(l > 210.0f) l = 210.0f;
    voice[vl].ff = l * l * iFs;
    voice[vl].f0 = voice[vl].f1 = 0.0f;

    voice[vl].note = note; //note->pan
    if(note <  12) note = 12;
    if(note > 108) note = 108;
    l = volume * trim;
    voice[vl].outr = l + l * width * (float)(note - 60);
    voice[vl].outl = l + l - voice[vl].outr;

    if(note < 44) note = 44; //limit max decay length
    l = 2.0f * param[0];
    if(l < 1.0f) l += 0.25f - 0.5f * param[0];
    voice[vl].dec = (float)exp(-iFs * exp(-0.6 + 0.033 * (double)note - l));
  }
  else //note off
  {
    // TODO: move the loop to mdaPiano.cpp
    for(v=0; v<NVOICES; v++) if(voice[v].note==note) //any voices playing that note?
    {
      if(sustain==0)
      {
        if(note < 94 || note == SUSTAIN) //no release on highest notes
          voice[v].dec = (float)exp(-iFs * exp(2.0 + 0.017 * (double)note - 2.0 * param[1]));
      }
      else voice[v].note = SUSTAIN;
    }
  }
}


void mdaPianoVoice::process(float **inputs, float **outputs, uint32_t sampleFrames)
{
  float* out0 = outputs[0];
  float* out1 = outputs[1];
  uint32_t event=0, frame=0, frames, v;
  float x, l, r;
  uint32_t i;

  while(frame<sampleFrames)
  {
    frames = notes[event++];
    if(frames>sampleFrames) frames = sampleFrames;
    frames -= frame;
    frame += frames;

    while(--frames>=0)
    {
      VOICE *V = voice;
      l = r = 0.0f;

      for(v=0; v<activevoices; v++)
      {
        V->frac += V->delta;  //integer-based linear interpolation
        V->pos += V->frac >> 16;
        V->frac &= 0xFFFF;
        if(V->pos > V->end) V->pos -= V->loop;

        i = waves[V->pos];
        i = (i << 7) + (V->frac >> 9) * (waves[V->pos + 1] - i) + 0x40400000;
        x = V->env * (*(float *)&i - 3.0f);  //fast int->float

        /////////////////////
        //TODO: This was used in processReplacing instead of the above
        /*
        //i = (i << 7) + (V->frac >> 9) * (waves[V->pos + 1] - i) + 0x40400000;   //not working on intel mac !?!
  i = waves[V->pos] + ((V->frac * (waves[V->pos + 1] - waves[V->pos])) >> 16);
  x = V->env * (float)i / 32768.0f;
        //x = V->env * (*(float *)&i - 3.0f);  //fast int->float
        */
        /////////////////////

        V->env = V->env * V->dec;  //envelope
        V->f0 += V->ff * (x + V->f1 - V->f0);  //muffle filter
        V->f1 = x;

        l += V->outl * V->f0;
        r += V->outr * V->f0;

        //TODO: this was used in processReplacing
        /////////////////////
        /*
 if(!(l > -2.0f) || !(l < 2.0f))
 {
   printf("what is this shit?   %d,  %f,  %f\n", i, x, V->f0);
   l = 0.0f;
 }
if(!(r > -2.0f) || !(r < 2.0f))
 {
   r = 0.0f;
 }
        */
        /////////////////////

        V++;
      }
      comb[cpos] = l + r;
      ++cpos &= cmax;
      x = cdep * comb[cpos];  //stereo simulator

      // TODO: processReplacing simply assigned instead of adding
      *out0++ += l + x;
      *out1++ += r - x;
    }

    if(frame<sampleFrames)
    {
      uint32_t note = notes[event++];
      uint32_t vel  = notes[event++];
      noteOn(note, vel);
    }
  }
  for(v=0; v<activevoices; v++) if(voice[v].env < SILENCE) voice[v] = voice[--activevoices];
  notes[0] = EVENTS_DONE;  //mark events buffer as done
}


void mdaPianoVoice::update(Param par)
{
  //TODO: use local copy
  float * param = programs[curProgram].param;
  size = (uint32_t)(12.0f * param[2] - 6.0f);
  sizevel = 0.12f * param[3];
  muffvel = param[5] * param[5] * 5.0f;

  velsens = 1.0f + param[6] + param[6];
  if(param[6] < 0.25f) velsens -= 0.75f - 3.0f * param[6];

  fine = param[9] - 0.5f;
  random = 0.077f * param[10] * param[10];
  stretch = 0.000434f * (param[11] - 0.5f);

  cdep = param[7] * param[7];
  trim = 1.50f - 0.79f * cdep;
  width = 0.04f * param[7];  if(width > 0.03f) width = 0.03f;

  poly = 8 + (uint32_t)(24.9f * param[8]);
}
