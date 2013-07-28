# LV2 port of the popular mdaPiano and mdaEPiano VSTi

Based on the mdaPiano / mdaEPiano sources
- Source: [`mda-vst-src-2010-02-14.zip`](http://sourceforge.net/projects/mda-vst/)
- Author: Paul Kellett (paul.kellett@mda-vst.com)


----------------------

## About

This repository holds the code to build the mdaEPiano and mdaPiano plugins for
the LV2 plugin framework.

I'd like to thank Paul Kellett for his decision to put the
VST plugins' source code under a free license (see LICENSE for more info).


## Compiling

To compile the mda Piano plugin, just execute the following statement:

    make && sudo make install

This will copy the compiled plugin to `/usr/local/lib/lv2`.
To build the mda EPiano plugin, pass `TYPE=mdaEPiano` to `make`:

    make TYPE=mdaEPiano
    sudo make install TYPE=mdaEPiano

**NOTE**: you will need the [lvtoolkit libraries](http://lvtoolkit.org) to
compile this plugin.


## Connecting

At the current stage you will need an LV2 host to use the plugin. I'm using
`jalv.gtk`:

    jalv.gtk http://elephly.net/lv2/mdaPiano

You should now be able to connect your MIDI device to the plugin via JACK. Also
don't forget to connect the plugin's stereo output to your system's outputs:

    jack_connect system:playback_1 mdaPiano:left
    jack_connect system:playback_2 mdaPiano:right

I provide a simple connect script with the code which does these things
automatically.


## License

The original mda VST plug-ins by Paul Kellett are released under the
MIT license or under the GPL "either version 2 of the License, or (at
your option) any later version". The LV2 ports of the mdaPiano and
mdaEPiano plugins by rekado are released under the same licenses.
