// Attention Circuits Control laboratory - Nunchuck adapter
// Status LED routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "neurochuck_includes.h"



//
// Private Variables

// We never poll on these, so "volatile" isn't needed.
uint32_t pattern;
uint32_t sweep_mask;



//
// Functions


// Initializes LED blink state.

void InitLED(void)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    pattern = BLINK_OFF;
    sweep_mask = 0x00;
    UpdateLED_ISR();
  }
}


// Switches to the specified blink pattern. Most-significant is emitted first.

void SetLED(uint32_t new_pattern)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    pattern = new_pattern;
    // Leave the sweep mask at its old value.
  }
}


// Cycles to the next LED state.

void UpdateLED_ISR(void)
{
  // We're called from an ISR, so no lock needed.

  if (0 == sweep_mask)
    sweep_mask = 0x80000000ul;

  if (pattern & sweep_mask)
    PIN_LED_PORT |= PIN_LED_BIT;
  else
    PIN_LED_PORT &= ~PIN_LED_BIT;

  sweep_mask >>= 1;
}


//
// This is the end of the file.
