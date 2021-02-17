// Attention Circuits Control laboratory - Nunchuck adapter
// Event queue
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Global variables

// Scheduled events.
// Anything that ISRs queue or handle should be volatile.
// Times are in ticks.

// Log message event.
extern bool log_queued;
extern uint32_t next_log;


//
// Functions

// Initializes pins that events use.
void InitEventPins(void);

// Initializes the event queue.
void InitEventQueue(void);

// Handles queued digital I/O. This should be fast (interrupt-friendly).
void PollEventsISR(void);

// Handles event reporting. This is slow (interrupt-unfriendly).
void PollEventReporting(void);


//
// This is the end of the file.
