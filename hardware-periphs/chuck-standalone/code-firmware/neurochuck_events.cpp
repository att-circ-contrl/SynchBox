// Attention Circuits Control laboratory - Nunchuck adapter
// Event queue
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "neurochuck_includes.h"


//
// Global variables

// Event queue.
// Anything that ISRs queue or handle should be volatile.
// Times are in ticks.

// Log message event.
bool log_queued;
uint32_t next_log;



//
// Private variables

// Event state and bookkeeping information.
// Times are in ticks.

// Log message event.
// No state tracking needed, as this is an instantaneous event.




//
// Functions


// Initializes the event queue.

void InitEventQueue(void)
{
  // ISRs may modify these, so we need to force this to be atomic.

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // Log messages.

    log_queued = false;
    next_log = 0;
  }
}



// Initializes pins that events use.

void InitEventPins(void)
{
  // Set pin states.
  // NOTE - This is done by register-twiddling, not the Arduino way.
  // All ports default to inputs.

  // Make sure pin pull-ups are enabled globally, if we're using the
  // Arduino IDE. Our homebrewed initialization does this already.
#ifndef USE_NEURAVR
  // FIXME - This is specific to m2560 and m328p! Other chips may vary!
  MCUCR = 0x00;
#endif

  // Set data-direction bits.
  // Doing this via read-modify-write is acceptable.
  PIN_LED_DIRECT |= PIN_LED_BIT;
  // PIN_AUTOSTART is already an input.
#if BOARD_TRINKET_POWER_KLUDGE
  PIN_FAKEPOWER_DIRECT |= (PIN_FAKEPOWER_LOW_BIT | PIN_FAKEPOWER_HIGH_BIT);
#endif

  // Set initial output values.
  // Remember to set the "autostart" strap output high, to enable pull-up.
  PIN_LED_PORT &= ~PIN_LED_BIT;
  PIN_AUTOSTART_PORT |= PIN_AUTOSTART_BIT;
#if BOARD_TRINKET_POWER_KLUDGE
  PIN_FAKEPOWER_PORT &= ~PIN_FAKEPOWER_LOW_BIT;
  PIN_FAKEPOWER_PORT |= PIN_FAKEPOWER_HIGH_BIT;
#endif
}



// Handles queued digital I/O. This should be fast (interrupt-friendly).
// FIXME - This may still take a while to run.
// This works with a timer tick of 10 kHz, but isn't guaranteed above that.

void PollEventsISR(void)
{
  uint32_t thistime;
  static uint16_t led_downsample = 0;

  // This is called from within an ISR, so it's already atomic (no wrapper
  // needed).

  // Get a timestamp.
// FIXME - This is never used, so don't bother.
//  thistime = QueryTimer_ISR();

  // Call the LED update routine at appropriate intervals.
  led_downsample++;
  if (RTC_TICKS_PER_LED_TICK <= led_downsample)
  {
    led_downsample = 0;
    UpdateLED_ISR();
  }

  // Done.
}



// Handles event reporting. This is slow (interrupt-unfriendly).

void PollEventReporting(void)
{
  uint32_t thistime;
  bool logging_active, nc_present;
  bool need_report;
  uint8_t nc_joy_x, nc_joy_y, nc_acc_x, nc_acc_y, nc_acc_z;
  bool nc_button_c, nc_button_z;
  char button_packed;
  uint16_t scratch;


  // Check to see what's updated.
  // Copy data atomically, to be ISR-update-friendly.

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // Record the timestamp.
    thistime = QueryTimer_ISR();

    // Make note of baseline state.
    logging_active = log_queued;
    nc_present = have_chuck;

    // Logging.
    need_report = false;
    if (log_queued)
    {
      if (thistime >= next_log)
      {
        need_report = true;
        next_log += log_period;
        // Handle the firehose case gracefully.
        if (next_log < thistime)
          next_log = thistime;
      }
    }

    // Nunchuck state.

    nc_joy_x = chuck_joy_x;
    nc_joy_y = chuck_joy_y;

    nc_button_c = chuck_button_c;
    nc_button_z = chuck_button_z;

    // Use a scratch variable for this to defer type change.
    scratch = chuck_acc_x >> 8;
    nc_acc_x = (uint8_t) (scratch & 0xff);
    scratch = chuck_acc_y >> 8;
    nc_acc_y = (uint8_t) (scratch & 0xff);
    scratch = chuck_acc_z >> 8;
    nc_acc_z = (uint8_t) (scratch & 0xff);
  }



  // Emit only at logged intervals.

  // Logging.
  if (need_report)
  {
    // Always do this in terse mode.

    if (want_timestamp)
    {
      PrintConstString("T");
      PrintHex32(thistime);
    }

    PrintConstString("J");
    PrintHex8(nc_joy_x);
    PrintHex8(nc_joy_y);

    button_packed = '0' + (nc_button_c ? 1 : 0) + (nc_button_z ? 2 : 0);
    PrintChar(button_packed);

    PrintConstString("L");
    PrintHex8(nc_acc_x);
    PrintHex8(nc_acc_y);
    PrintHex8(nc_acc_z);

    PrintConstString("\r\n");
  }


  // Update the blink code state.
  // NOTE - This means SetLED() _cannot_ reset the blink bit position, as
  // we call it repeatedly even when not changing the pattern.

  if (!want_blink)
    SetLED(BLINK_OFF);  // LED is dark when not used.
  else if (!nc_present)
    SetLED(BLINK_BLINK);  // LED blinks on and off when no nunchuck.
  else if (logging_active)
    SetLED(BLINK_PULSE);  // LED gives pulse blips when logging.
  else
    SetLED(BLINK_NOTCH);  // LED is on with dark blips when idle.
}


//
// This is the end of the file.
