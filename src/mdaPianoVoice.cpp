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

  reset();
}


void mdaPianoVoice::reset() {
  env = 0.0f;
  dec = 0.99f;
  muff = 160.0f;
  volume = 0.2f;
}


float mdaPianoVoice::p_helper(unsigned short id, Param d) {
  if (d == Default)
    return default_preset[p_offset(id)];
  else
    return *p(id);
}


mdaPianoVoice::on(unsigned char note, unsigned char velocity)
{
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
