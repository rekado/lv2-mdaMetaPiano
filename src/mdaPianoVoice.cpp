#include "mdaPianoVoice.h"

mdaPianoVoice::mdaPianoVoice(double rate, Sample * master_samples, KGRP * master_kgrp) {
  //set tuning
  Fs = rate;
  iFs = 1.0f/Fs;

  samples = master_samples;
  kgrp  = master_kgrp;

  // TODO: load default values from ttl file
#ifdef PIANO
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
#elif defined EPIANO
  default_preset[p_offset(p_envelope_decay)]       = 0.500f;
  default_preset[p_offset(p_envelope_release)]     = 0.500f;
  default_preset[p_offset(p_hardness)]             = 0.500f;
  default_preset[p_offset(p_treble_boost)]         = 0.500f;
  default_preset[p_offset(p_modulation)]           = 0.500f;
  default_preset[p_offset(p_lfo_rate)]             = 0.650f;
  default_preset[p_offset(p_velocity_sensitivity)] = 0.250f;
  default_preset[p_offset(p_stereo_width)]         = 0.500f;
  default_preset[p_offset(p_polyphony)]            = 0.500f; // unused
  default_preset[p_offset(p_fine_tuning)]          = 0.500f;
  default_preset[p_offset(p_random_tuning)]        = 0.146f;
  default_preset[p_offset(p_overdrive)]            = 0.000f;
#endif

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
  long k, s;

  if(velocity > 0)
  {
    k = (key - 60) * (key - 60);
    l = fine + random * ((float)(k % 13) - 6.5f);  //random & fine tune
#ifdef PIANO
    if(key > 60) l += stretch * (float)k; //stretch
#endif

    s = size;
#ifdef PIANO
    if(velocity > 40) s += (long)(sizevel * (float)(velocity - 40));
#endif

    k = 0;
    while(key > (kgrp[k].high + s)) k += SAMPLES_PER_NOTE; // find keygroup
    sample_index = k; // store sample index

    l += (float)(key - kgrp[k].root); // pitch
#ifdef PIANO
    l = 22050.0f * iFs * (float)exp(0.05776226505 * l);
#elif defined EPIANO
    l = 32000.0f * iFs * (float)exp(0.05776226505 * l);
#endif
    delta = (long)(65536.0f * l);
    frac = 0;
    pos = 0;

#ifdef EPIANO
    if(velocity > 48) sample_index++; // mid velocity sample
    if(velocity > 80) sample_index++; // high velocity sample
#endif

#ifdef PIANO
    end = samples[sample_index].size;
#elif defined EPIANO
    end = samples[sample_index].size - 1;
#endif
    loop = kgrp[sample_index].loop;


#ifdef PIANO
    env = (0.5f + velsens) * (float)pow(0.0078f * velocity, velsens); //velocity

    l = 50.0f + *p(p_muffling_filter) * *p(p_muffling_filter) * muff + muffvel * (float)(velocity - 64); //muffle
    if(l < (55.0f + 0.25f * (float)key)) l = 55.0f + 0.25f * (float)key;
    if(l > 210.0f) l = 210.0f;
    ff = l * l * iFs;
    f0 = f1 = 0.0f;
#elif defined EPIANO
    env = (3.0f + 2.0f * velsens) * (float)pow(0.0078f * velocity, velsens); // velocity
    // high notes quieter
    if(key > 60) {
      env *= (float)exp(0.01f * (float)(60 - key));
    }
#endif

    // note->pan
    if(key <  12) key = 12;
    if(key > 108) key = 108;

#ifdef PIANO
    l = volume * trim;
    outr = l + l * width * (float)(key - 60);
    outl = l + l - outr;

    if(key < 44) key = 44; //limit max decay length
    l = 2.0f * *p(p_envelope_decay);
    if(l < 1.0f) l += 0.25f - 0.5f * *p(p_envelope_decay);
    dec = (float)exp(-iFs * exp(-0.6 + 0.033 * (double)key - l));
#elif defined EPIANO
    outr = volume + volume * width * (float)(key - 60);
    outl = volume + volume - outr;

    if(key < 44) key = 44; // limit max decay length
    dec = (float)exp(-iFs * exp(-1.0 + 0.03 * (double)key - 2.0f * *p(p_envelope_decay)));
#endif
  } else {
    // some keyboards send note off events as 'note on' with velocity 0
    release(0);
  }
}


void mdaPianoVoice::reset() {
  env = 0.0f;
  dec = 0.99f;
  sustain = 0;
  volume = 0.2f;
#ifdef PIANO
  muff = 160.0f;
  cpos = 0;
#elif defined EPIANO
  tl = tr = lfo0 = dlfo = 0.0f;
  lfo1 = 1.0f;
#endif
  m_key = lvtk::INVALID_KEY;
}


void mdaPianoVoice::release(unsigned char velocity) {
  if(sustain==0) {
#ifdef PIANO
    //no release on highest notes
    if(m_key < 94 || m_key == SUSTAIN) {
      dec = (float)exp(-iFs * exp(2.0 + 0.017 * (double)m_key - 2.0 * *p(p_envelope_release)));
    }
#elif defined EPIANO
    dec = (float)exp(-iFs * exp(6.0 + 0.01 * (double)m_key - 5.0 * *p(p_envelope_release)));
#endif
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
  long i;

  update(Current);
  for (uint32_t frame = from; frame < to; ++frame) {
    // initialize left and right output
    l = r = 0.0f;

    frac += delta;  // integer-based linear interpolation
    pos += frac >> 16;
    frac &= 0xFFFF;  // why AND it with all ones?
    if(pos > end) pos -= loop; // jump back to loop sample

#ifdef PIANO
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
#elif defined EPIANO
    // alternative method (probably faster), said to not work on intel mac
    //i = samples[sample_index].buffer[pos];
    //i = (i << 7) + (frac >> 9) * (samples[sample_index].buffer[pos + 1] - i) + 0x40400000;

    i = samples[sample_index].buffer[pos] + ((frac * (samples[sample_index].buffer[pos + 1] - samples[sample_index].buffer[pos])) >> 16);
    x = env * (float)i / 32768.0f;
    env = env * dec;  // envelope

    // overdrive
    if(x>0.0f) {
      x -= overdrive * x * x;
      if(x < -env)
        x = -env;
    }

    l += outl * x;
    r += outr * x;

    // treble boost
    tl += tfrq * (l - tl);
    tr += tfrq * (r - tr);
    r  += treb * (r - tr);
    l  += treb * (l - tl);

    lfo0 += dlfo * lfo1; // LFO for tremolo and autopan
    lfo1 -= dlfo * lfo0;
    l += l * lmod * lfo1;
    r += r * rmod * lfo1; // TODO: worth making all these local variables?

    // write to output
    p(p_left)[frame] += l;
    p(p_right)[frame] += r;
#endif
  }

  // turn off further processing when the envelope has rendered the voice silent
  if (env < SILENCE) {
    m_key = lvtk::INVALID_KEY;
  }

#ifdef EPIANO
  if(fabs(tl)<1.0e-10) tl = 0.0f; // anti-denormal
  if(fabs(tr)<1.0e-10) tr = 0.0f;
#endif
}


void mdaPianoVoice::update(Param par) {
#ifdef PIANO
  size = (long)(12.0f * p_helper(p_hardness_offset, par) - 6.0f);
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
#elif defined EPIANO
  size = (long)(12.0f * p_helper(p_hardness, par) - 6.0f);
  treb = 4.0f * p_helper(p_treble_boost, par) * p_helper(p_treble_boost, par) - 1.0f; // treble gain

  // treble frequency
  if(p_helper(p_treble_boost, par) > 0.5f) {
    tfrq = 14000.0f;
  } else {
    tfrq = 5000.0f;
  }
  tfrq = 1.0f - (float)exp(-iFs * tfrq);

  rmod = lmod = p_helper(p_modulation, par) + p_helper(p_modulation, par) - 1.0f; // lfo depth
  if(p_helper(p_modulation, par) < 0.5f) rmod = -rmod;
  dlfo = 6.283f * iFs * (float)exp(6.22f * p_helper(p_lfo_rate, par) - 2.61f); // lfo rate

  velsens = 1.0f + p_helper(p_velocity_sensitivity, par) + p_helper(p_velocity_sensitivity, par);
  if(p_helper(p_velocity_sensitivity, par) < 0.25f) {
    velsens -= 0.75f - 3.0f * p_helper(p_velocity_sensitivity, par);
  }

  width = 0.03f * p_helper(p_stereo_width, par);
  fine = p_helper(p_fine_tuning, par) - 0.5f;
  random = 0.077f * p_helper(p_random_tuning, par) * p_helper(p_random_tuning, par);
  overdrive = 1.8f * p_helper(p_overdrive, par);
#endif
}
