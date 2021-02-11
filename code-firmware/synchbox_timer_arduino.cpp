// Attention Circuits Control laboratory - I/O SynchBox project
// Timer routines - Mega 2560 - Arduino IDE compatibility implementation.
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "synchbox_includes.h"



// FIXME - Wrapper.
// This code should only be used if we have _not_ defined USE_NEURAVR.

#ifndef USE_NEURAVR



//
// Private macros

// Timer control registers are TCCRnA, TCCRnB, and TIMSKn.
// Make appropriate macros for bit patterns for each.
// These are different for 8- and 16-bit timers.
// We can ignore TCCRnC. This is used to force compare matches while running.

// TCCRA gets a constant value. CTC mode, no output pins.
#define TCCRA_8_CONST   (0b0010)
#define TCCRA_16_CONST  (0)

// TCCRB's value depends on whether it's timer 0, timer 2, or a 16-bit timer.
// Among other things, the high-order bits of the timer mode are in B for 16-bit.
// Fortunately if all we want to do is turn them off it doesn't vary much.
#define TCCRB_8_OFF    (0)
#define TCCRB_16_OFF  (0b01000)

// Turning timer interrupts off is also easy.
#define TIMSK_8_OFF   (0)
#define TIMSK_16_OFF  (0)


//
// Private variables

// RTC timestamp counter.
// This is volatile, as the ISR needs to update it.
volatile uint32_t rtc_timestamp;


//
// Functions


// Unhooks the Arduino timers and initializes our timer.

void Arduino_InitTimer(void)
{
  uint32_t clocks_per_tick;
  uint32_t scratch;

  // Disable all timers and timer interrupts.
  // Arduino always uses 0, and sometimes uses 1 and 2, depending on what libraries are loaded.

  TIMSK0 = TIMSK_8_OFF;
  TIMSK1 = TIMSK_16_OFF;
  TIMSK2 = TIMSK_8_OFF;
  TIMSK3 = TIMSK_16_OFF;
  TIMSK4 = TIMSK_16_OFF;
  TIMSK5 = TIMSK_16_OFF;

  TCCR0A = TCCRA_8_CONST;
  TCCR0B = TCCRB_8_OFF;

  TCCR1A = TCCRA_16_CONST;
  TCCR1B = TCCRB_16_OFF;

  TCCR2A = TCCRA_8_CONST;
  TCCR2B = TCCRB_8_OFF;

  TCCR3A = TCCRA_16_CONST;
  TCCR3B = TCCRB_16_OFF;

  TCCR4A = TCCRA_16_CONST;
  TCCR4B = TCCRB_16_OFF;

  TCCR5A = TCCRA_16_CONST;
  TCCR5B = TCCRB_16_OFF;


  // Initialize the clock.
  ResetTimer();


  // Turn on our timer. We're using timer 5.
  // The only interrupt we care about is output compare A.

  // We're not using a prescaler.
  // This will get rounded, but it's as close as we're going to get.
  clocks_per_tick = CPU_SPEED / RTC_TICKS_PER_SECOND;
  // Convert this to a compare-match value.
  // For a /1 prescaler, f = cpuclk / (1 + OCRnA).
  // So, OCRnA = (cpuclk / f) - 1.
  // Sanity-check the "can't go this fast" case.
  if (0 < clocks_per_tick)
    clocks_per_tick--;
  // Clamp this to 65535, for the "can't go this slow" case.
  if (0xffff < clocks_per_tick)
    clocks_per_tick = 0xffff;

  // Initialize.
  // NOTE: Type coercion has to be done carefully here.
  // Otherwise we can get truncation before we want it.
  scratch = (clocks_per_tick >> 8) & 0xff;
  OCR5AH = (uint8_t) scratch;
  scratch = clocks_per_tick & 0xff;
  OCR5AL = (uint8_t) scratch;

  // We don't care about OCRnB or the counter value.
  // The first is ignored and the second will stabilize in at most 8 ms.

  // Timer active, prescaler /1.
  TCCR5B = 0b01001;
  // Interrupts enabled.
  TIMSK5 = (1 << OCIE5A);
}



// Clears the real-time clock timestamp.
// NOTE - This must be called from within an ISR or atomic block.

void Arduino_ResetTimer(void)
{
  rtc_timestamp = 0;
}



// Reads the real-time clock timestamp.
// We have locking (normal) and non-locking (ISR) versions.

uint32_t Arduino_QueryTimer(void)
{
  uint32_t thistime;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    thistime = rtc_timestamp;
  }

  return thistime;
}


uint32_t Arduino_QueryTimer_ISR(void)
{
  return rtc_timestamp;
}



// Interrupt service routine.
// All this does is update the RTC timestamp.
// We only care about the comare match A interrupt.

ISR(TIMER5_COMPA_vect, ISR_BLOCK)
{
  // This will overflow after about 400 hours at 10 kHz.
  // Overflow is a tolerable failure mode.
  rtc_timestamp++;

  // Handle digital I/O due to events. This should be fast.
  // FIXME - This may still take a while to run. The timer works at 10 kHz,
  // but at 100 kHz all bets are off.
  PollEventsISR();
}



// FIXME - Wrapper ends.
#endif


//
// This is the end of the file.
