// Attention Circuits Control laboratory - Nunchuck adapter
// I2C routines - Arduino implementation
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "neurochuck_includes.h"



// FIXME - Wrapper.
// This code should only be used if we have _not_ defined USE_NEURAVR.

#ifndef USE_NEURAVR


//
// Functions


// Initializes I2C communication.

void Arduino_I2CInit(void)
{
  Wire.begin();
}



// Performs a burst write, blocking until complete.
// Returns true on success and false on failure.

bool Arduino_I2CBurstWrite(uint8_t addr, uint8_t *data, int count)
{
  bool isok;
  int num_written;
  int cidx;

  isok = true;

  Wire.beginTransmission(addr);

  // Write the whole burst even if a failure occurs.
  for (cidx = 0; cidx < count; cidx++)
  {
    num_written = Wire.write(data[cidx]);
    if (1 != num_written)
      isok = false;
  }

  Wire.endTransmission();

  return isok;
}



// Performs a burst read, blocking until complete.
// Returns true on success and false on failure.

bool Arduino_I2CBurstRead(uint8_t addr, volatile uint8_t *data, uint8_t count)
{
  bool isok;
  int num_read;
  int recidx;
  uint8_t thisbyte;

  isok = true;

  // Apparently this does the whole burst read and buffers it for us.
  num_read = Wire.requestFrom(addr, count);

  recidx = 0;
  while (Wire.available())
  {
    thisbyte = Wire.read();

    if (recidx < count)
      data[recidx] = thisbyte;

    recidx++;
  }

  if ((num_read != count) || (recidx != count))
    isok = false;

  return isok;
}


// FIXME - Wrapper ends.
#endif


//
// This is the end of the file.
