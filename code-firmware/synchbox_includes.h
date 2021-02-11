// Attention Circuits Control laboratory - I/O SynchBox project
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

#endif


// Project-specific includes.
#include <util/atomic.h>
#include "synchbox_config.h"
#include "synchbox_timer.h"
#include "synchbox_adc.h"
#include "synchbox_light.h"
#include "synchbox_io.h"
#include "synchbox_events.h"
#include "synchbox_parse.h"
#include "synchbox_host.h"
#include "synchbox_main.h"


//
// This is the end of the file.
