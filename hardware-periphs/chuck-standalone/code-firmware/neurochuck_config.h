// Attention Circuits Control laboratory - Nunchuck adapter
// Configuration values and switches
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Diagnostics constants

#define VERSION_STR "20170810"

#define DEVICETYPE "Neurochuck"
#define DEVICESUBTYPE "v1"

// This toggles availability of debugging commands.
#define DEBUG_AVAILABLE 0


//
// Pin configuration constants

// Configuration switches for board type.
#define BOARD_PRO_TRINKET 1
#define BOARD_PRO_TRINKET_5V 1

#define BOARD_NANO 0

// Run power from I/O lines, per Ben and Marcus's sample unit.
#define BOARD_TRINKET_POWER_KLUDGE 1

// Use the built-in Arduino status LED.
#define LED_USE_BUILTIN 1


//
// Nunchuck configuration constants

// Scrambled (Wii) vs unscrambled (Wii + clones) mode.
#define USE_SCRAMBLED_MODE 0


// Digital I/O lines.
// These use AVR numberings.

// NOTE - The explicit cast is important, so that we don't waste a lot
// of time taking the complement when masking out a bit.

#if BOARD_PRO_TRINKET

// Fake power rails for the nunchuck adapter.
// On the Pro Trinket, these are C2/C3 (Ain 2/3).
#define PIN_FAKEPOWER_PORT PORTC
#define PIN_FAKEPOWER_DIRECT DDRC
#define PIN_FAKEPOWER_LOW_BIT ( (uint8_t) (1 << 2) )
#define PIN_FAKEPOWER_HIGH_BIT ( (uint8_t) (1 << 3) )

// Pull-down strap for auto-startup.
// On the Pro Trinket, this is B0 (Dio8).
#define PIN_AUTOSTART_PORT PORTB
#define PIN_AUTOSTART_INPUT PINB
#define PIN_AUTOSTART_DIRECT DDRB
#define PIN_AUTOSTART_BIT ( (uint8_t) (1 << 0) )

// Status LED pin.
// On the Pro Trinket, this is either the built-in (B5/SCK/Dio13) or B1 (Dio9).
#define PIN_LED_PORT PORTB
#define PIN_LED_DIRECT DDRB
#if LED_USE_BUILTIN
#define PIN_LED_BIT ( (uint8_t) (1 << 5) )
#else
#define PIN_LED_BIT ( (uint8_t) (1 << 1) )
#endif

// BOARD_PRO_TRINKET
#endif

#if BOARD_NANO

// FIXME - Nano NYI.

// BOARD_NANO
#endif


//
// Host link constants

// Host link baud rate.
// 115.2 and 500.0 have decent hardware support.
#define HOST_BAUD 115200
//define HOST_BAUD 500000

// Default settings vary depending on whether we're in automatic or manual
// mode.

// Manual mode.

#define DEFAULT_MANUAL_ECHO 1
#define DEFAULT_MANUAL_TSTAMP 1
#define DEFAULT_MANUAL_LOG_INTERVAL 1000 /* 10 Hz */
#define DEFAULT_MANUAL_BLINK 1
#define DEFAULT_MANUAL_POLL_INTERVAL 100 /* 100 Hz */

// Autostart mode.

#define DEFAULT_AUTO_ECHO 0
#define DEFAULT_AUTO_TSTAMP 0
#define DEFAULT_AUTO_LOG_INTERVAL 0 /* As fast as possible. */
#define DEFAULT_AUTO_BLINK 1
#define DEFAULT_AUTO_POLL_INTERVAL 0 /* As fast as possible. */

// NOTE - We're not using verbosity settings. Output is always terse.


//
// Timing constants


// Stuff that doesn't vary from board to board.

// RTC ticks per second.
// Match the Neurarduino's convention.
#define RTC_TICKS_PER_SECOND 10000ul

// RTC ticks per LED-control tick.
// Sampling rate is 16 Hz.
#define RTC_TICKS_PER_LED_TICK 625


// Board-specific stuff.

#if BOARD_PRO_TRINKET

// The clock speed depends on the voltage.
#if BOARD_PRO_TRINKET_5V
#define CPU_SPEED 16000000ul
#else
#define CPU_SPEED 12000000ul
#endif

// BOARD_PRO_TRINKET
#endif

#if BOARD_NANO

// FIXME - Nano NYI.

// BOARD_NANO
#endif


//
// This is the end of the file.
