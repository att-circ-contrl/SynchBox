// Attention Circuits Control laboratory - Nunchuck adapter
// Main entry-points
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.

//
// Includes

#include "neurochuck_includes.h"


//
// Functions


// Global setup entry-point.

void DoAllSetup(void)
{
  InitEventPins();
  InitEventQueue();
  InitLED();
  InitChuck();
  InitHostLink();

  // The timer gets set up _last_, as this triggers the ISR.
  InitTimer();
}



// Polling loop task hook (called once per loop).

void DoAllPollingTasks(void)
{
  // None of these are real-time, which is why they're called from the
  // polling loop instead of the ISR.
  PollHostLink();
  PollChuck(); // May or may not block, depending on implementation.
  PollEventReporting();
}



//
// Main Program


// This is only needed if we're _not_ building via the Arduino IDE.

#ifdef USE_NEURAVR

int main(void)
{
  // One-time initialization.

  MCU_Init();
  DoAllSetup();

  // Loop forever.
  while (1)
  {
    DoAllPollingTasks();
  }

  // We should never reach here.
  // Report success.
  return 0;
}

#endif



//
// This is the end of the file.
