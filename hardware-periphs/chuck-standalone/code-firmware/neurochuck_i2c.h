// Attention Circuits Control laboratory - Nunchuck adapter
// I2C routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Macros

// These use local code by default, and neuravr library functionality if
// USE_NEURAVR is defined.

#ifdef USE_NEURAVR


// NeurAVR firmware calls.

// FIXME - NYI.

// Initializes I2C communication.
#define WrapperI2CInit() /* Do nothing. */

// Performs a burst write, blocking until complete.
// Returns true on success and false on failure.
#define WrapperI2CWriteBurstBlocking(X, Y, Z) false

// Performs a burst read, blocking until complete.
// Returns true on success and false on failure.
#define WrapperI2CReadBurstBlocking(X, Y, Z) false


#else


// Local code for Arduino IDE builds.

// Initializes I2C communication.
#define WrapperI2CInit() Arduino_I2CInit()

// Performs a burst write, blocking until complete.
// Returns true on success and false on failure.
#define WrapperI2CWriteBurstBlocking(X, Y, Z) Arduino_I2CBurstWrite(X, Y, Z)

// Performs a burst read, blocking until complete.
// Returns true on success and false on failure.
#define WrapperI2CReadBurstBlocking(X, Y, Z) Arduino_I2CBurstRead(X, Y, Z)


#endif


//
// Functions

#ifdef USE_NEURAVR


// FIXME - NYI.


#else


// Local code wraps Arduino's "wire" library.
// FIXME - There are some reports of this hanging, when a device is absent or
// misbehaving.

// Initializes I2C communication.
void Arduino_I2CInit(void);

// Performs a burst write, blocking until complete.
// Returns true on success and false on failure.
bool Arduino_I2CBurstWrite(uint8_t addr, uint8_t *data, int count);

// Performs a burst read, blocking until complete.
// Returns true on success and false on failure.
bool Arduino_I2CBurstRead(uint8_t addr, volatile uint8_t *data, uint8_t count);


#endif


//
// This is the end of the file.
