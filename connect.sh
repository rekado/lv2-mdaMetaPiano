#!/bin/bash

TYPE=${1:-mdaPiano}

# connect MIDI mate II to MIDI through (Alsa MIDI)
aconnect 24:0 14:0

# connect to JACK
jack_connect $TYPE:midi "a2j:Midi Through [14] (capture): Midi Through Port-0"

jack_connect system:playback_1 $TYPE:left
jack_connect system:playback_2 $TYPE:right
