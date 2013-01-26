#include "mdaPianoVoice.h"

mdaPianoVoice::mdaPianoVoice(double rate, Sample * master_samples, KGRP * master_kgrp) {
  //set tuning
  Fs = rate;
  iFs = 1.0f/Fs;

  samples = master_samples;
  kgrp  = master_kgrp;

  default_preset[p_offset(p_envelope_decay)]       = 0.500f;
  default_preset[p_offset(p_envelope_release)]     = 0.500f;
  default_preset[p_offset(p_hardness_offset)]      = 0.500f;
  default_preset[p_offset(p_velocity_to_hardness)] = 0.500f;
  default_preset[p_offset(p_muffling_filter)]      = 0.803f;
  default_preset[p_offset(p_velocity_to_muffling)] = 0.251f;
  default_preset[p_offset(p_velocity_sensitivity)] = 0.376f;
  default_preset[p_offset(p_stereo_width)]         = 0.500f;
  default_preset[p_offset(p_polyphony)]            = 0.330f; // unused
  default_preset[p_offset(p_fine_tuning)]          = 0.500f;
  default_preset[p_offset(p_random_detuning)]      = 0.246f;
  default_preset[p_offset(p_stretch_tuning)]       = 0.500f;

  reset();
  update(Default);
}


float mdaPianoVoice::p_helper(unsigned short id, Param d) {
  if (d == Default)
    return default_preset[p_offset(id)];
  else
    return *p(id);
}


void mdaPianoVoice::on(unsigned char key, unsigned char velocity) {
  // store key that turned this voice on (used in 'get_key')
  m_key = key;
  update(Current);

  float l=99.0f;
  uint32_t k, s;

  if(velocity > 0)
  {
    k = (key - 60) * (key - 60);
    l = fine + random * ((float)(k % 13) - 6.5f);  //random & fine tune
    if(key > 60) l += stretch * (float)k; //stretch

    s = size;
    if(velocity > 40) s += (uint32_t)(sizevel * (float)(velocity - 40));

    k = 0;
    while(key > (kgrp[k].high + s)) k += SAMPLES_PER_NOTE; // find keygroup
    sample_index = k; // store sample index

    l += (float)(key - kgrp[k].root); // pitch
    l = 22050.0f * iFs * (float)exp(0.05776226505 * l);
    delta = (uint32_t)(65536.0f * l);
    frac = 0;
    pos = 0;
    end = samples[sample_index].size;
    loop = kgrp[sample_index].loop;

    env = (0.5f + velsens) * (float)pow(0.0078f * velocity, velsens); //velocity

    l = 50.0f + *p(p_muffling_filter) * *p(p_muffling_filter) * muff + muffvel * (float)(velocity - 64); //muffle
    if(l < (55.0f + 0.25f * (float)key)) l = 55.0f + 0.25f * (float)key;
    if(l > 210.0f) l = 210.0f;
    ff = l * l * iFs;
    f0 = f1 = 0.0f;

    // note->pan
    if(key <  12) key = 12;
    if(key > 108) key = 108;
    l = volume * trim;
    outr = l + l * width * (float)(key - 60);
    outl = l + l - outr;

    if(key < 44) key = 44; //limit max decay length
    l = 2.0f * *p(p_envelope_decay);
    if(l < 1.0f) l += 0.25f - 0.5f * *p(p_envelope_decay);
    dec = (float)exp(-iFs * exp(-0.6 + 0.033 * (double)key - l));
  } else {
    // some keyboards send note off events as 'note on' with velocity 0
    release(0);
  }
}


void mdaPianoVoice::reset() {
  env = 0.0f;
  dec = 0.99f;
  muff = 160.0f;
  volume = 0.2f;
  sustain = 0;
  cpos = 0;
  m_key = lvtk::INVALID_KEY;
}


void mdaPianoVoice::release(unsigned char velocity) {
  if(sustain==0) {
    //no release on highest notes
    if(m_key < 94 || m_key == SUSTAIN) {
      dec = (float)exp(-iFs * exp(2.0 + 0.017 * (double)m_key - 2.0 * *p(p_envelope_release)));
    }
  } else {
    m_key = SUSTAIN;
  }

  //Mark the voice to be turned off later. It may not be set to
  //INVALID_KEY yet, because the release sound still needs to be
  //rendered.  m_key is finally set to INVALID_KEY by 'render' when
  //env < SILENCE
  m_key = SUSTAIN;
}


void mdaPianoVoice::render(uint32_t from, uint32_t to) {
  // abort if no key is pressed
  // initially m_key is INVALID_KEY, so no sound will be rendered
  if (m_key == lvtk::INVALID_KEY)
    return;

  float x, l, r;
  uint32_t i;

  update(Current);
  for (uint32_t frame = from; frame < to; ++frame) {
    // initialize left and right output
    l = r = 0.0f;

    frac += delta;  // integer-based linear interpolation
    pos += frac >> 16;
    frac &= 0xFFFF;
    if(pos > end) pos -= loop;

    i = samples[sample_index].buffer[pos];
    i = (i << 7) + (frac >> 9) * (samples[sample_index].buffer[pos + 1] - i) + 0x40400000;
    x = env * (*(float *)&i - 3.0f);  //fast int->float

    env = env * dec;  //envelope
    f0 += ff * (x + f1 - f0);  //muffle filter
    f1 = x;

    l += outl * f0;
    r += outr * f0;

    comb[cpos] = l + r;
    ++cpos &= 0x7F;
    x = cdep * comb[cpos];  //stereo simulator

    // write to output
    p(p_left)[frame] += l + x;
    p(p_right)[frame] += r - x;
  }

  // turn off further processing when the envelope has rendered the voice silent
  if (env < SILENCE) {
    m_key = lvtk::INVALID_KEY;
  }
}


void mdaPianoVoice::update(Param par) {
  size = (uint32_t)(12.0f * p_helper(p_hardness_offset, par) - 6.0f);
  sizevel = 0.12f * p_helper(p_velocity_to_hardness, par);
  muffvel = p_helper(p_velocity_to_muffling, par) * p_helper(p_velocity_to_muffling, par) * 5.0f;

  velsens = 1.0f + p_helper(p_velocity_sensitivity, par) + p_helper(p_velocity_sensitivity, par);
  if(p_helper(p_velocity_sensitivity, par) < 0.25f) {
    velsens -= 0.75f - 3.0f * p_helper(p_velocity_sensitivity, par);
  }

  fine = p_helper(p_fine_tuning, par) - 0.5f;
  random = 0.077f * p_helper(p_random_detuning, par) * p_helper(p_random_detuning, par);
  stretch = 0.000434f * (p_helper(p_stretch_tuning, par) - 0.5f);

  cdep = p_helper(p_stereo_width, par) * p_helper(p_stereo_width, par);
  trim = 1.50f - 0.79f * cdep;
  width = 0.04f * p_helper(p_stereo_width, par);  if(width > 0.03f) width = 0.03f;
}
