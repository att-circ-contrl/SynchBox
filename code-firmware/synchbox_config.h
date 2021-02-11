// Attention Circuits Control laboratory - I/O SynchBox project
// Configuration values and switches
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Diagnostics constants

#define VERSION_STR "20210211"

#define DEVICETYPE "I/O SynchBox"
#define DEVICESUBTYPE "v1"

// This toggles availability of debugging commands.
#define DEBUG_AVAILABLE 1
// Debug trigger dead time (to avoid noise-induced re-triggering).
// This is in ticks.
#define DEBUG_DEADTIME 10
// Length of the debugging reward pulse. This is in ticks.
#define DEBUG_REWARDLENGTH 1000


//
// Pin configuration constants

// Digital outputs.

// Reward is on Arduino D11, which is AVR B5.
// Second reward is on Arduino D10, which is AVR B4.
// Timing is on Arduino D12, which is AVR B6.
// Second timing is on Arduino D13, which is AVR B7.
// LED is on Arduino D4, which is AVR G5.
// Formerly on Arduino D8, which is AVR H5.
// NOTE - This is ATMega2560-specific!

// NOTE - The explicit cast is important, so that we don't waste a lot
// of time taking the complement when masking out a bit.

#define REWARD_PORT PORTB
#define REWARD_DIRECTION DDRB
#define REWARD_BIT ( (uint8_t) (1 << 5) )
#define REWARD_DESC "D11/B5/GP05"

#define REWARD2_PORT PORTB
#define REWARD2_DIRECTION DDRB
#define REWARD2_BIT ( (uint8_t) (1 << 4) )
#define REWARD2_DESC "D10/B4/GP04"

#define TIMING_PORT PORTB
#define TIMING_DIRECTION DDRB
#define TIMING_BIT ( (uint8_t) (1 << 6) )
#define TIMING_DESC "D12/B6/GP06"

#define TIMING2_PORT PORTB
#define TIMING2_DIRECTION DDRB
#define TIMING2_BIT ( (uint8_t) (1 << 7) )
#define TIMING2_DESC "D13/B7/GP07"

#define LED_PORT PORTG
#define LED_DIRECTION DDRG
#define LED_BIT ( (uint8_t) (1 << 5) )

// Additional digital I/Os for debugging.
// These are in the v2 case's GPIO block.
// Debug trigger is on Arduino D6, AVR H3, GPIO 0.
// Debug monitor is on Arduino D7, AVR H4, GPIO 1.

#define DEBUGTRIG_PORT PORTH
#define DEBUGTRIG_INPUT PINH
#define DEBUGTRIG_DIRECTION DDRH
#define DEBUGTRIG_BIT ( (uint8_t) (1 << 3) )
#define DEBUGTRIG_DESC "D6/H3/GP00"

#define DEBUGMON_PORT PORTH
#define DEBUGMON_DIRECTION DDRH
#define DEBUGMON_BIT ( (uint8_t) (1 << 4) )
#define DEBUGMON_DESC "D7/H4/GP01"

// Event code handshaking header.
// These are Atmel I/O register IDs.
// NOTE - This is ATMega2560-specific!
// C0..C7 is pin 37..30, L0..L7 is pin 49..42.
// Setting things up so that Data15..Data0 is pins 30..37,42..49.
// To strobe, just flicker the MSB.
#define EVCODE_PORT_H PORTC
#define EVCODE_PORT_L PORTL
#define EVCODE_DDR_H DDRC
#define EVCODE_DDR_L DDRL

// Analog inputs.
// These are Arudino analog pin IDs.
// NOTE - The Arduino's analog IDs match the ATmega2560's ADC input IDs.
// This is ATmega2560-specific!
#define JOY_X_PIN 0
#define JOY_Y_PIN 1
#define JOY_C_PIN 2
#define OPT_L_PIN 4
#define OPT_R_PIN 5

// Analog precision: 8 or 16 bits.
// The full scale range corresponds to 0V..2.56V in both cases.
#define ANALOG_16BIT 0

// Noise suppression filter.
// This has an exponential smoothing filter for LPF, and outlier rejection for
// spike suppression.
// The exponential smoothing filter gives new samples 1 part in 2^n weight.
// The outlier rejection filter rejects samples whose squared distance is 2^m
// times the variance. It uses its own LPF for both its averege and variance.
// Setting either LPF tuning parameter to 0 disables that filter.
// There are 5 analog channels, with round-robin sampling every 2 ticks. This
// means the native sampling rate per channel is 1 kHz.
#define AN_FILTER_LPF_BITS_DEFAULT 2
#define AN_FILTER_SPIKE_LPF_BITS_DEFAULT 6
#define AN_FILTER_SPIKE_SIZE_BITS_DEFAULT 2


//
// Host link constants

// Host link baud rate.
// 115.2 can be done with high precision. 230.4 with coarser precision.
// 500.0 can be done with high precision and is supported. 250 isn't.
// FIXME - 230 is unreliable with stock hardware. Ditto with an FTDI dongle.
#define HOST_BAUD 115200
//define HOST_BAUD 230400
//define HOST_BAUD 500000

// Echo.
#define ECHO_DEFAULT 1

// Default verbosity and reporting interval (in ticks).
#define DEFAULT_VERBOSITY VERB_FULL
// FIXME - Set a safe logging rate and let the user modify it.
// The eye-tracker runs at 300 Hz. NOTE - The closest we can get is 303.
// Verbose output at 115.2 tops out around 150 Hz.
#define DEFAULT_LOGINTERVAL 100 /* 100 Hz */
//define DEFAULT_LOGINTERVAL 33 /* 303.03... Hz */


//
// Timing constants

// CPU speed is the same for any of the AVR Arduino boards that we'd use.
// Make it a macro instead of an inline constant in case we need to swap out crystals for UART reasons.
#define CPU_SPEED 16000000ul

// Number of RTC ticks per second.
// We want better precision than milliseconds, but only want a 32-bit timestamp; so, custom ticks.
#define RTC_TICKS_PER_SECOND 10000ul
// FIXME - Debugging value that won't alias much with the standard value.
//define RTC_TICKS_PER_SECOND 6899ul

// Nested interrupt toggle.
// We don't need this at 115.2 kbaud, but do need it at 500 kbaud, so that
// we can service multiple UART interrupts per tick.
#define ALLOW_NESTED_INTERRUPTS 1



//
// This is the end of the file.
