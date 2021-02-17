// Attention Circuits Control Laboratory - Nunchuck adapter
// Host command parsing
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Functions


// Initializes the parser.
void InitParser();


// Entry point for handling text from the host.
// The raw string must be either NULL or guaranteed NULL-terminated.

void ProcessInputLine(char *thisline);



//
// This is the end of the file.
