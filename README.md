# LV2 port of the popular mdaPiano and mdaEPiano VSTi

Port
  Author: rekado

Based on the mdaPiano / mdaEPiano sources
  Source: `mda-vst-src-2010-02-14.zip`
  Author: Paul Kellett (paul.kellett@mda-vst.com)
          http://sourceforge.net/projects/mda-vst/

----------------------

## About

This is an attempt to merge the mdaEPiano and mdaPiano ports.  The two
plugins share a lot of code.  Putting them together and building both
plugins from the same source should make maintenance simpler.

I'd like to thank Paul Kellett for his decision to put the
plugins' source code under a free license (see LICENSE for more info).


## Compiling

To compile the plugin, just execute the following statement:

    make && sudo make install

This will copy the compiled plugin to `/usr/local/lib/lv2`.

**NOTE**: you will need the lvtoolkit libraries to compile this plugin: http://lvtoolkit.org


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

The original mda VST plug-ins by Paul Kellett are released under the
MIT license or under the GPL "either version 2 of the License, or (at
your option) any later version". The LV2 ports of the mdaPiano and
mdaEPiano plugins by rekado are released under the same licenses.
