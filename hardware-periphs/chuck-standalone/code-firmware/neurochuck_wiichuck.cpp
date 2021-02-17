// Attention Circuits Control laboratory - Nunchuck adapter
// Wii-nunchuck routines.
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "neurochuck_includes.h"



//
// Public Variables


// Nunchuck state variables.
bool have_chuck;
uint8_t chuck_joy_x, chuck_joy_y;
bool chuck_button_c, chuck_button_z;
// The 10 most-significant bits are data; the rest are zeroes.
uint16_t chuck_acc_x, chuck_acc_y, chuck_acc_z;



//
// Private Macros

#define CHUCK_ADDR 0x52

#define CHUCK_REGSET_SIZE 2
#define CHUCK_REQUEST_SIZE 1
#define CHUCK_DATA_SIZE 6

#define CHUCK_DETECT_POLL_TICKS 1000 /* 10 Hz */



//
// Private Enums

// FIXME - Extend this to cover the non-blocking version's states.
enum chuck_state_t
{
  CHUCK_NOT_FOUND,
  CHUCK_READY
};



//
// Private Constants

// These can't actually be "const", as they're passed as "uint8 *" arguments.
uint8_t chuck_wii_init_string[CHUCK_REGSET_SIZE] = { 0x40, 0x00 };
uint8_t chuck_clone_init_a_string[CHUCK_REGSET_SIZE] = { 0xf0, 0x55 };
uint8_t chuck_clone_init_b_string[CHUCK_REGSET_SIZE] = { 0xfb, 0x00 };
uint8_t chuck_req_string[CHUCK_REQUEST_SIZE] = { 0x00 };



//
// Private Variables

// State machine information for talking to the nunchuck.
chuck_state_t chuck_state = CHUCK_NOT_FOUND;

// Last data poll time.
uint32_t last_poll_time;

// Received raw data.
volatile uint8_t chuck_raw_data[CHUCK_DATA_SIZE];



//
// Private Prototypes


// Initializes the raw data array to a consistent state.
void ClearChuckRawData(void);

// Parses the raw data array, updating cooked data values.
void ProcessChuckRawData(void);



//
// Functions

// Sets up initial nunchuck state. This should return quickly.

void InitChuck(void)
{
  // Initialize I2C.
  WrapperI2CInit();


  // Initialize timing.
  last_poll_time = QueryTimer();


  // Initialize data.

  have_chuck = false;
  chuck_state = CHUCK_NOT_FOUND;

  chuck_joy_x = 0x00;
  chuck_joy_y = 0x00;

  chuck_button_c = false;
  chuck_button_z = false;

  chuck_acc_x = 0x00;
  chuck_acc_y = 0x00;
  chuck_acc_z = 0x00;

  ClearChuckRawData();


  // NOTE - There's lengthy initialization needed for the nunchuck.
  // Do this as part of polling (detection).

  // Nunchuck detection state is updated when we try to poll it,
  // so don't do detection here.
}



// Initializes the raw data array to a consistent state.

void ClearChuckRawData(void)
{
  int idx;

  for (idx = 0; idx < CHUCK_DATA_SIZE; idx++)
    chuck_raw_data[idx] = 0;
}


// Parses the raw data array, updating cooked data values.

void ProcessChuckRawData(void)
{
  int idx;
  uint8_t scratch8;
  uint16_t scratch16;

  // Descramble everything.
#if USE_SCRAMBLED_MODE
  for (idx = 0; idx < CHUCK_DATA_SIZE; idx++)
  {
    // FIXME - Blithely assume this wraps the addition properly.
    scratch8 = chuck_raw_data[idx];
    scratch8 ^= 0x17;
    scratch8 += 0x17;
    chuck_raw_data[idx] = scratch8;
  }
#endif

  // Copy relevant information.

  chuck_joy_x = chuck_raw_data[0];
  chuck_joy_y = chuck_raw_data[1];

  chuck_acc_x = chuck_raw_data[2];
  chuck_acc_y = chuck_raw_data[3];
  chuck_acc_z = chuck_raw_data[4];

  scratch16 = chuck_raw_data[5];

  // Remember that buttons are negative-logic in hte raw data stream.
  chuck_button_c = true;
  if (scratch16 & 0b0010)
    chuck_button_c = false;

  chuck_button_z = true;
  if (scratch16 & 0b0001)
    chuck_button_z = false;

  chuck_acc_x <<= 8;
  chuck_acc_x |= (scratch16 & 0b00001100) << 4;

  chuck_acc_y <<= 8;
  chuck_acc_y |= (scratch16 & 0b00110000) << 2;

  chuck_acc_z <<= 8;
  chuck_acc_z |= (scratch16 & 0b11000000);
}



// Polls the nunchuck, waiting for a response.

void PollChuck(void)
{
  bool isok;
  uint32_t thistime;

  isok = true;

  // Fetch the timestamp.
  thistime = QueryTimer();

  // Take action depending on the current state.
  // FIXME - We may need to add wait states in here, for non-blocking behavior.
  switch (chuck_state)
  {
    case CHUCK_NOT_FOUND:
      // If it's time for another detection attempt, try to initialize.
      if ( (thistime - last_poll_time) >= CHUCK_DETECT_POLL_TICKS )
      {
        last_poll_time = thistime;

        // Send the handshake/initialization signal.
#if USE_SCRAMBLED_MODE
        isok = WrapperI2CWriteBurstBlocking(CHUCK_ADDR,
          chuck_wii_init_string, CHUCK_REGSET_SIZE);
#else
        isok = WrapperI2CWriteBurstBlocking(CHUCK_ADDR,
          chuck_clone_init_a_string, CHUCK_REGSET_SIZE);

        if (isok)
          isok = WrapperI2CWriteBurstBlocking(CHUCK_ADDR,
            chuck_clone_init_b_string, CHUCK_REGSET_SIZE);
#endif

        if (isok)
        {
          have_chuck = true;
          chuck_state = CHUCK_READY;

          // Queue the next poll.
          isok = WrapperI2CWriteBurstBlocking(CHUCK_ADDR, chuck_req_string,
            CHUCK_REQUEST_SIZE);
        }
      }
      break;

    case CHUCK_READY:
      // If it's time for another data poll, ask for data.
      if ( (thistime - last_poll_time) >= chuck_poll_period )
      {
        last_poll_time = thistime;

        ClearChuckRawData();

        if (isok)
        {
          isok = WrapperI2CReadBurstBlocking(CHUCK_ADDR, chuck_raw_data,
            CHUCK_DATA_SIZE);

          if (isok)
          {
            ProcessChuckRawData();

            // Queue the next poll.
            isok = WrapperI2CWriteBurstBlocking(CHUCK_ADDR, chuck_req_string,
              CHUCK_REQUEST_SIZE);
          }
        }
      }
      break;
  }

  // If an I2C error occurred, assume the nunchuck is unplugged.
  if (!isok)
  {
    have_chuck = false;
    chuck_state = CHUCK_NOT_FOUND;
  }
}


//
// This is the end of the file.
