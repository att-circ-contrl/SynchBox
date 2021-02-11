// Attention Circuits Control laboratory - I/O SynchBox project
// Host command parsing
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "synchbox_includes.h"



//
// Private macros

// Maximum length of an actual command word (opcode).
#define MAX_OPCODE_CHARS 3



//
// Private enums

enum parse_result_t
{
  PARSER_EMPTY,
  PARSER_VALID,
  PARSER_BAD
};

enum parse_state_t
{
  STATE_PREAMBLE,
  STATE_OPCODE,
  STATE_GAP,
  STATE_ARGUMENT,
  STATE_TAIL,
  STATE_ERROR
};



//
// Private variables

// Parsed command.
char opcode[MAX_OPCODE_CHARS];
bool argvalid;
uint32_t argument;



//
// Private prototypes


// Initializes command-parsing output.
void InitOpCommand();

// Checks to see if the contents of the command buffer are valid.
parse_result_t ValidateCommand();

// Prints a short "unrecognized command" message.
void PrintShortHelp();



//
// Functions


// Initializes the parser.

void InitParser()
{
  // Not much to do here.
  InitOpCommand();
}



// Initializes command-parsing output.

void InitOpCommand()
{
  int idx;

  for (idx = 0; idx < MAX_OPCODE_CHARS; idx++)
    opcode[idx] = 0;

  argvalid = false;
  argument = 0;
}



// Checks to see if the contents of the command buffer are valid.
// The raw string is guaranteed NULL-terminated.

parse_result_t ValidateCommand(char *rawcommand)
{
  parse_result_t result;
  parse_state_t state;
  int rawidx, opidx;
  char thischar;
  bool special_case_help;

  result = PARSER_EMPTY;


  // Scan the command string, testing against "^\s*\w+\s+(\d+)?\s*$",
  // more or less. The opcode can be up to three letters, nothing else.

  InitOpCommand();
  state = STATE_PREAMBLE;
  opidx = 0;
  special_case_help = false;
  thischar = 0;

  for (rawidx = 0;
    0 != (thischar = rawcommand[rawidx]);
    rawidx++)
  {
    // First pass: update the state based on what we're looking at.
    switch (state)
    {
      case STATE_PREAMBLE:
        if ( (('A' <= thischar) && ('Z' >= thischar))
          || (('a' <= thischar) && ('z' >= thischar)) )
          state = STATE_OPCODE;
        else if (' ' < thischar)
        {
          state = STATE_ERROR;

          if ('?' == thischar)
            special_case_help = true;
        }
        break;

      case STATE_OPCODE:
        if ( (('A' <= thischar) && ('Z' >= thischar))
          || (('a' <= thischar) && ('z' >= thischar)) )
          // Keep the same state.
          state = STATE_OPCODE;
        else if (' ' >= thischar)
          state = STATE_GAP;
        else
          state = STATE_ERROR;
        break;

      case STATE_GAP:
        if (('0' <= thischar) && ('9' >= thischar))
          state = STATE_ARGUMENT;
        else if (' ' < thischar)
          state = STATE_ERROR;
        break;

      case STATE_ARGUMENT:
        if (('0' <= thischar) && ('9' >= thischar))
          // Keep the same state.
          state = STATE_ARGUMENT;
        else if (' ' >= thischar)
          state = STATE_TAIL;
        else
          state = STATE_ERROR;
        break;

      case STATE_TAIL:
        if (' ' < thischar)
          state = STATE_ERROR;
        break;

      default:
        // Error state; nothing more to do.
        break;
    }

    // Second pass: We know what we're looking at. Update it.
    switch (state)
    {
      case STATE_OPCODE:
        // Convert lower case to upper case.
        if (('a' <= thischar) && ('z' >= thischar))
        {
          thischar -= 'a';
          thischar += 'A';
        }
        // Add this character, if there's room for it.
        if (opidx < MAX_OPCODE_CHARS)
        {
          opcode[opidx] = thischar;
          opidx++;
        }
        else
          state = STATE_ERROR;
        break;

      case STATE_ARGUMENT:
        argvalid = true;
        argument *= 10;
        thischar -= '0';
        argument += (uint32_t) thischar;
        break;

      default:
        // Whatever this is, there's no further processing to do.
        break;
    }

    // Finished handling this character.
  }

  // Third pass: Special-case "?", turning it into "HLP".
  // This will have been rejected due to non-alphabetical opcode characters.
  if (special_case_help)
  {
    // Squash any processing we've done.
    InitOpCommand();

    // Manually stuff the buffer, pretending we got "HLP" and then "<cr>".
    opidx = 3;
    opcode[0] = 'H';
    opcode[1] = 'L';
    opcode[2] = 'P';

    state = STATE_OPCODE;
  }

  // Fourth pass: Make sure we had _exactly_ MAX_OPCODE_CHARS in the
  // opcode, if we had anything at all.
  if ( (STATE_PREAMBLE != state) && (opidx != MAX_OPCODE_CHARS) )
    state = STATE_ERROR;


  // Figure out whether we had something valid or not.

  if (STATE_ERROR == state)
  {
    InitOpCommand();
    result = PARSER_BAD;
  }
  else if (STATE_PREAMBLE == state)
    result = PARSER_EMPTY;
  else
    result = PARSER_VALID;


  // Done.
  return result;
}



// Prints a short "unrecognized command" message.
// The raw string is guaranteed NULL-terminated.

void PrintShortHelp(char *rawcommand)
{
  int idx;
  char thischar;

  PrintConstString("Unrecognized command:  \"");

  // Take this apart character by character, in case there are unprintable
  // characters in the string.
  for (idx = 0;
    0 != (thischar = rawcommand[idx]);
    idx++)
  {
    if ((32 <= thischar) && (126 >= thischar))
    {
      PrintChar(thischar);
    }
    else
    {
      PrintChar('<');
      PrintHex8(thischar);
      PrintChar('>');
    }
  }

  PrintConstString("\". Type \"?\" or \"HLP\" for help.\r\n");
}



// Entry point for handling text from the host.
// The raw string must be either NULL or guaranteed NULL-terminated.

void ProcessInputLine(char *thisline)
{
  parse_result_t validity;
  bool is_ok;

  if (NULL != thisline)
  {
    // We have a new line of input. Attempt to process it.

    // See if we can parse this.
    validity = ValidateCommand(thisline);

    // If this looks legit, act on it. Otherwise report it.
    is_ok = true;
    if (PARSER_VALID == validity)
    {
      is_ok = HandleHostCommand(opcode, argument, argvalid);
    }

    if ( (PARSER_BAD == validity) || (!is_ok) )
      PrintShortHelp(thisline);
  }
}



//
// This is the end of the file.
