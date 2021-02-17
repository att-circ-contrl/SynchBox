// Attention Circuits Control laboratory - Nunchuck adapter
// Wii-nunchuck routines.
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Public Variables

// Nunchuck state variables.
extern bool have_chuck;
extern uint8_t chuck_joy_x, chuck_joy_y;
extern bool chuck_button_c, chuck_button_z;
// The 10 most-significant bits are data; the rest are zeroes.
extern uint16_t chuck_acc_x, chuck_acc_y, chuck_acc_z;


//
// Functions

// Sets up initial nunchuck state. This should return quickly.
void InitChuck(void);

// Polls the nunchuck, waiting for a response.
// This may block until new data is present (the Arduino way), or may do
// a non-blocking state machine update (modifying the chuck variables only
// when new data does arrive).
void PollChuck(void);


//
// This is the end of the file.
