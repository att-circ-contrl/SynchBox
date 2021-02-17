// Attention Circuits Control laboratory - Nunchuck adapter
// Timer routines - Mega 328p
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Macros

// These use local code by default, and neuravr library functionality if
// USE_NEURAVR is defined.

#ifdef USE_NEURAVR


// NeurAVR firmware calls.

// Initializes our timer and sets up our ISR callback.
#define InitTimer() DoTimerInit()

// Clears the real-time clock timestamp.
#define ResetTimer() Timer_Reset()

// Reads the real-time clock timestamp.
// We have locking (normal) and non-locking (ISR) versions.
#define QueryTimer() Timer_Query()
#define QueryTimer_ISR() Timer_Query_ISR()


#else


// Local code for Arduino IDE builds.

// Initializes our timer. ISR callback is hardwired into the ISR.
#define InitTimer() Arduino_InitTimer()

// Clears the real-time clock timestamp.
#define ResetTimer() Arduino_ResetTimer()

// Reads the real-time clock timestamp.
// We have locking (normal) and non-locking (ISR) versions.
#define QueryTimer() Arduino_QueryTimer()
#define QueryTimer_ISR() Arduino_QueryTimer_ISR()


#endif


//
// Functions

#ifdef USE_NEURAVR


// Timer initialization needs multiple function calls, so there's a wrapper
// function when using the NeurAVR library.
void DoTimerInit(void);


#else


// Local code is pretty much a duplicate of the relevant NeurAVR library
// code.


// Initializes our timer. ISR callback is hardwired into the ISR.
void Arduino_InitTimer(void);

// Clears the real-time clock timestamp.
// NOTE - This must be called from within an ISR or atomic block.
void Arduino_ResetTimer(void);

// Reads the real-time clock timestamp.
// We have locking (normal) and non-locking (ISR) versions.
uint32_t Arduino_QueryTimer(void);
uint32_t Arduino_QueryTimer_ISR(void);


#endif


//
// This is the end of the file.
