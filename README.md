# LV2 port of the famous mdaPiano VSTi

Port
  Author: rekado
  Based on: `mda-vst-src-2010-02-14.zip`

mdaPiano
  Author: Paul Kellett (paul.kellett@mda-vst.com)
          http://sourceforge.net/projects/mda-vst/

----------------------

## About

This is an attempt to port the mdaPiano VSTi plugin to the LV2 plugin
platform. I'd like to thank Paul Kellett for his decision to put the
plugin's source code under a free license (see LICENSE for more info).


## Compiling

To compile the plugin, just execute the following statement:

    make && sudo make install

This will copy the compiled plugin to `/usr/local/lib/lv2`.

**NOTE**: you will need larsl's `lv2-c++-tools` to compile this plugin: http://freecode.com/projects/lv2-c-tools


## Connecting

At the current stage you will need an LV2 host to use the plugin. I'm using
`lv2_jack_host` during development:

    lv2_jack_host http://elephly.net/lv2/mdaPiano

You should now be able to connect your MIDI device to the plugin via JACK. Also
don't forget to connect the plugin's stereo output to your system's outputs:

    jack_connect system:playback_1 mdaPiano:left
    jack_connect system:playback_2 mdaPiano:right

At least this is what I do on my system. I provide a simple connect script with
the code which does these things automatically.


## License

The original mda VST plug-ins by Paul Kellett are released under the MIT
license or under the GPL "either version 2 of the License, or (at your option)
any later version". The LV2 port of the mdaPiano plugin by rekado is released
under the same licenses.
