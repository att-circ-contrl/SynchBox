// Attention Circuits Control laboratory - I/O SynchBox project
// Main Arduino project file
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.

//
// Includes

#include "synchbox_includes.h"


//
// Arduino entrypoints


// These wrap the relevant functions from synchbox_main.cpp.


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
