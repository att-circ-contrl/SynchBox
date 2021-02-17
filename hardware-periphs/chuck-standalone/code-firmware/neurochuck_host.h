// Attention Circuits Control laboratory - Nunchuck adapter
// Host link routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Global variables

// Configurable behavior parameters.
// These are high-level parameters; the event queue uses low-level timestamps.

extern uint32_t log_period;
extern uint32_t chuck_poll_period;
extern bool want_timestamp;
extern bool want_blink;

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
