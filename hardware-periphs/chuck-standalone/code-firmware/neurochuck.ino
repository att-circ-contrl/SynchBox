// Attention Circuits Control laboratory - Nunchuck adapter
// Main Arduino project file
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.

//
// Includes

// Arduino library files have to go here as well, so that the IDE knows to 
// add them to the include/link paths.
#include <Wire.h>

// Primary project header file.
#include "neurochuck_includes.h"


//
// Arduino entrypoints


// These wrap the relevant functions from neuarduino_main.cpp.


void setup()
{
  DoAllSetup();
}


void loop()
{
  DoAllPollingTasks();
}


//
// This is the end of the file.
