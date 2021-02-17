// Attention Circuits Control laboratory - Nunchuck adapter
// Top-level include file.
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes


// Platform-specific includes.
// If we're in the Arduino IDE, use "Arduino.h".
// If we're building manually with the NeurAVR library, use "neuravr.h".

#ifdef USE_NEURAVR

#include "neuravr.h"

#else

#include "Arduino.h"
#include <Wire.h>

#endif


// Project-specific includes.
#include <util/atomic.h>
#include "neurochuck_config.h"
#include "neurochuck_timer.h"
#include "neurochuck_i2c.h"
#include "neurochuck_led.h"
#include "neurochuck_io.h"
#include "neurochuck_wiichuck.h"
#include "neurochuck_events.h"
#include "neurochuck_parse.h"
#include "neurochuck_host.h"
#include "neurochuck_main.h"


//
// This is the end of the file.
