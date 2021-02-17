// Attention Circuits Control laboratory - Nunchuck adapter
// Status LED routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Macros

// Blink codes.

#define BLINK_OFF   0x00000000ul
#define BLINK_ON    0xfffffffful
#define BLINK_PULSE 0xa0000000ul
#define BLINK_NOTCH 0xdffffffful
#define BLINK_BLINK 0xff00ff00ul


//
// Functions

// Initializes LED blink state.
void InitLED(void);

// Switches to the specified blink pattern. Most-significant is emitted first.
void SetLED(uint32_t new_pattern);

// Cycles to the next LED state.
void UpdateLED_ISR(void);


//
// This is the end of the file.
