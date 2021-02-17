// Attention Circuits Control laboratory - Nunchuck adapter
// Timer routines - Mega 328p - NeurAVR firmware version.
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "neurochuck_includes.h"



// FIXME - Wrapper.
// This code should only be used if USE_NEURAVR is defined.

#ifdef USE_NEURAVR



//
// Functions

// Initializes our timer and sets up our ISR callback.
void DoTimerInit(void)
{
  Timer_Init(CPU_SPEED, RTC_TICKS_PER_SECOND);

  // RTC clock ticks are already handled, so all we need is the event ISR.
  Timer_RegisterCallback(&PollEventsISR);
}



// FIXME - Wrapper ends.
#endif


//
// This is the end of the file.
