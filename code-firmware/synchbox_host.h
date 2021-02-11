// Attention Circuits Control laboratory - I/O SynchBox project
// Host link routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Enums

enum verbosity_t
{
  VERB_PACKED,
  VERB_TERSE,
  VERB_FULL
};


//
// Global variables

// Configurable behavior parameters.
// These are high-level parameters; the event queue uses low-level timestamps.

extern uint32_t log_period;

extern uint32_t timing_pulse_width;
extern uint32_t timing_pulse_period;

extern uint32_t timing2_pulse_width;
extern uint32_t timing2_pulse_period;

extern uint32_t evcode_config_setup;
extern uint32_t evcode_config_pulse;
extern uint32_t evcode_config_hold;
extern bool evcode_config_is_byte;
extern bool evcode_config_want_strobe;

extern verbosity_t verbosity;

extern bool debug_light_samples;

extern bool debug_trig_reward;
extern bool debug_trig_timing;
extern bool debug_trig_evcode;

extern bool debug_monitor_analog;
extern uint16_t debug_monitor_analog_channel;
extern char debug_monitor_analog_labels[DEBUG_ANALOG_CHANCOUNT];


//
// Functions

// Initializes communication with the host.
void InitHostLink();

// Processes buffered incoming characters from the host.
// Handles at most one command, to avoid delaying reports.
void PollHostLink();

// Entry point for processing a completed, hopefully-valid host command.
// Returns true if successful and false if not.
bool HandleHostCommand(char *opcode, uint16_t argument, bool argvalid);



//
// This is the end of the file.
