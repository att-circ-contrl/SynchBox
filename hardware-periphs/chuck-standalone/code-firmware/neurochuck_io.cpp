// Attention Circuits Control laboratory - Nunchuck adapter
// I/O wrapper routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "neurochuck_includes.h"


//
// Macros and constants

// Length of the line buffer, if we're doing this manually.
#define LINE_BUFFER_SIZE 20

// Length of the scratch buffer used for building hex numbers.
#define HEX_BUFFER_SIZE 10



//
// Private variables

#ifdef USE_NEURAVR

#else

char linebuffer[LINE_BUFFER_SIZE];
int charcount;

char hexscratch[HEX_BUFFER_SIZE];

#endif



//
// Private prototypes


#ifdef USE_NEURAVR

#else

// Clears the contents of the line buffer.
void ClearLineBuffer(void);

#endif



//
// Functions


#ifdef USE_NEURAVR

#else


// Sets up the serial port and clears the receive buffer.

void Arduino_SerialInit(uint32_t baud)
{
  ClearLineBuffer();

  Serial.begin(baud);
}



// Clears the contents of the line buffer.

void ClearLineBuffer(void)
{
  for (charcount = 0; charcount < LINE_BUFFER_SIZE; charcount++)
    linebuffer[charcount] = 0;
  charcount = 0;
}



// This adds pending characters to a string buffer, returning the buffer
// when a complete line is detected.
// This is non-blocking, returning NULL if we haven't found end of line yet.

char *Arduino_GetNextLine(void)
{
  char *result;
  int rawserial;
  char thischar;
  bool done;

  done = false;
  result = NULL;

  while ( (!done) && (0 < Serial.available()) )
  {
    rawserial = Serial.read();
    thischar = (char) rawserial;

    if ( ('\n' == thischar) || ('\r' == thischar) )
    {
      // End of line.
      done = true;

      // No matter what, terminate the string.
      linebuffer[LINE_BUFFER_SIZE - 1] = 0;
      if (charcount < LINE_BUFFER_SIZE)
        linebuffer[charcount] = 0;

      // If we have a non-empty command, return it.
      // Otherwise assume we're seeing a blank line or part of a CRLF pair.
      if (0 < charcount)
        result = linebuffer;
    }
    else if (' ' > thischar)
    {
      // This is a control character. Ignore it.
    }
    else
    {
      // Assume this is legit.
      if (charcount < LINE_BUFFER_SIZE)
      {
        linebuffer[charcount] = thischar;
        charcount++;
      }
    }
  }

  return result;
}



// This relinquishes the line buffer, reinitializing it for future calls to
// Arduino_GetNextLine().

void Arduino_DoneWithLine(void)
{
  // Just stomp the buffer.
  ClearLineBuffer();
}



// Prints a fixed-width zero-padded hexadecimal number.
// FIXME - Between type-promotion and snprintf, this may be slow.

void Arduino_PrintHex(uint32_t value, uint8_t width)
{
  // Shouldn't be needed but do this anyways.
  hexscratch[0] = 0;

  // Pick an appropriate pattern. Default to 32-bit.
  switch (width)
  {
    case 8:
      snprintf(hexscratch, HEX_BUFFER_SIZE, "%02lx", value);
      break;

    case 16:
      snprintf(hexscratch, HEX_BUFFER_SIZE, "%04lx", value);
      break;

    default:
      snprintf(hexscratch, HEX_BUFFER_SIZE, "%08lx", value);
      break;
  }

  // Pattern is built.
  Serial.print(hexscratch);
}


#endif


//
// This is the end of the file.
