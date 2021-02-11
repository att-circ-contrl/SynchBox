// Attention Circuits Control laboratory - I/O SynchBox project
// Event queue
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Macros

// Number of analog channels the debug monitor can listen to.
#define DEBUG_ANALOG_CHANCOUNT 5


//
// Global variables

// ADC round-robin rollover flag.
// This is volatile, because the ISR updates it.
extern volatile bool adc_new_round;

// FIXME - ADC "swapped joystick and light sensor" flag.
// This is advisory, set when the swap state changes.
// This is a workaround for filter hardware issues.
extern bool light_joystick_swapped;

// FIXME - Debugging threshold, expost so that we can tell the user.
extern uint16_t debug_threshold_middle_uint;

// Event queue.
// Times are in ticks.
// These are volatile, because ISRs can queue and/or handle some of them.

// Log message event.
// These aren't ISR-updated.
extern bool log_queued;
extern uint32_t next_log;

// Reward pulse event.

extern volatile bool reward_queued;
extern volatile uint32_t next_reward_on;
extern volatile uint32_t next_reward_off;

extern volatile bool reward2_queued;
extern volatile uint32_t next_reward2_on;
extern volatile uint32_t next_reward2_off;

// Timing pulse event.

extern volatile bool timing_queued;
extern volatile uint32_t next_timing_on;
extern volatile uint32_t next_timing_off;

extern volatile bool timing2_queued;
extern volatile uint32_t next_timing2_on;
extern volatile uint32_t next_timing2_off;

// Event code event.
extern volatile bool evcode_queued;
extern volatile uint32_t evcode_data_on;
extern volatile uint32_t evcode_data_off;
extern volatile uint32_t evcode_strobe_on;
extern volatile uint32_t evcode_strobe_off;
extern volatile uint16_t evcode_data_value;

//
// Functions

// Initializes the event queue.
void InitEventQueue(void);

// Initializes pins that events use.
void InitEventPins(void);

// Handles queued digital I/O. This should be fast (interrupt-friendly).
void PollEventsISR(void);

// Handles event reporting. This is slow (interrupt-unfriendly).
void PollEventReporting(void);

// Updates noise filter configuration.
void ConfigureNoiseFilters(uint8_t new_fuzz_bits, uint8_t new_var_bits,
  uint8_t new_spike_bits);

// Sets swapped/not-swapped state for light sensors and joysticks.
// FIXME - This is a workaround for filter hardware issues.
void SetOptJoySwapState(bool is_swapped);


//
// This is the end of the file.
