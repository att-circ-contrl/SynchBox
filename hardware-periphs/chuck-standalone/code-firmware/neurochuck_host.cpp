// Attention Circuits Control laboratory - Nunchuck adapter
// Host link routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes


#include "neurochuck_includes.h"



//
// Global variables


// Configurable behavior parameters.
// These are high-level parameters; the event queue has low-level ones.

// NOTE - Static initializers use the manual settings; at run-time, we
// check the auto-start strap and adjust if needed.

uint32_t log_period = DEFAULT_MANUAL_LOG_INTERVAL;
uint32_t chuck_poll_period = DEFAULT_MANUAL_POLL_INTERVAL;
bool want_timestamp = (bool) (DEFAULT_MANUAL_TSTAMP);
bool want_blink = (bool) (DEFAULT_MANUAL_BLINK);


//
// Private variables


// NOTE - Static initializers use the manual settings; at run-time, we
// check the auto-start strap and adjust if needed.

bool echo_active = (bool) (DEFAULT_MANUAL_ECHO);

// This is for diagnostics, mostly.
bool had_autostart = false;



//
// Private prototypes


// Prints a long help message.
void PrintLongHelp();

// Reports the full system state. Verbose.
void QueryState();



//
// Functions


// Initializes communication with the host.

void InitHostLink()
{
  uint8_t scratch;

  // Check the "autostart" strap state, and adjust config if needed.
  // Remember that this is negative logic (active-low).
  scratch = PIN_AUTOSTART_INPUT;
  if (!(scratch & PIN_AUTOSTART_BIT))
  {
    log_period = DEFAULT_AUTO_LOG_INTERVAL;
    chuck_poll_period = DEFAULT_AUTO_POLL_INTERVAL;
    want_timestamp = (bool) (DEFAULT_AUTO_TSTAMP);
    want_blink = (bool) (DEFAULT_AUTO_BLINK);
    echo_active = (bool) (DEFAULT_AUTO_ECHO);
    had_autostart = true;
    // NOTE - The timer shouldn't be running yet, so no lock needed.
    log_queued = true;
  }

  // Start the host link.
  InitSerialLink(CPU_SPEED, HOST_BAUD);

  // Set up parsing.
  InitParser();
}



// Processes buffered incoming characters from the host.
// Handles at most one command, to avoid delaying reports.

void PollHostLink()
{
  char *rawstring;

  // Get the most recently completed string. This may be NULL.
  rawstring = GetNextLine();

  if (NULL != rawstring)
  {
    if (echo_active)
    {
      PrintString(rawstring);
      PrintConstString("\r\n");
    }

    ProcessInputLine(rawstring);

    // Relinquish use of this line.
    DoneWithLine();
  }
}



// Entry point for processing a completed, hopefully-valid host command.
// Returns true if successful and false if not.

bool HandleHostCommand(char *opcode, uint16_t argument, bool argvalid)
{
  bool need_help;
  uint32_t scratch32;

  // Assume that the command is valid, by default.
  need_help = false;

  // Switch based on the command mnemonic.
  // This should already have been upper-cased for us.
  // To simplify things, the first character specifies the type of command.

  switch(opcode[0])
  {
    case '?':
    case 'H':
      // Display the help screen.
      // We don't care what the rest of the command is.
      PrintLongHelp();
    break;

    case 'Q':
      // Query the system state.
      // We don't care what the rest of the command is.
      QueryState();
    break;

    case 'E':
      // Turn host echo on or off.
      // There's only one command ("ECH n").
      if (!argvalid)
        need_help = true;
      else
      {
        if (0 == argument)
          echo_active = false;
        else
          echo_active = true;
      }
    break;

    case 'I':
      // This is either "INI" (initialize) or "IDQ" (identity query).
      if ('D' == opcode[1])
      {
        // Transmit device identification string.
        // This gives us a way to tell serial-usb AVR devices apart.
        PrintConstString(
          "devicetype: " DEVICETYPE
          "  subtype: " DEVICESUBTYPE
          "  revision: " VERSION_STR
          "\r\n"
        );
      }
      else if ('N' == opcode[1])
      {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // Initialize the event queue and reset the clock.
          InitEventQueue();
          ResetTimer();
          // Squash any outputs that may have been interrupted.
          InitEventPins();
          InitLED();
        }
      }
      else
        need_help = true;
    break;

    case 'L':
      // Log report management.
      // Command is "LIN n", "LTS n", or "LOG n".
      if (!argvalid)
        need_help = true;
      else if ('I' == opcode[1])
      {
        // All values, including zero, are valid; zero just firehoses.
        log_period = argument;
      }
      else if ('T' == opcode[1])
      {
        if (0 == argument)
          want_timestamp = false;
        else
          want_timestamp = true;
      }
      else if ('O' == opcode[1])
      {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          if (0 != argument)
          {
            next_log = QueryTimer_ISR();
            log_queued = true;
          }
          else
          {
            log_queued = false;
          }
        }
      }
      else
        need_help = true;
    break;

    case 'K':
      // Set the nunchuck polling interval.
      // There's only one command ("KIN n").
      if (!argvalid)
        need_help = true;
      else
        // All values, including zero, are valid; zero just firehoses.
        chuck_poll_period = argument;
    break;

    case 'B':
      // Turn blink codes on or off.
      // There's only one command ("BLN n").
      if (!argvalid)
        need_help = true;
      else
      {
        if (0 == argument)
          want_blink = false;
        else
          want_blink = true;
      }
    break;

#if DEBUG_AVAILABLE
    case 'X':
      // Debug ("examine" etc) functions.
      // FIXME - No debug commands implemented.
      need_help = true;
    break;
#endif

    default:
      // No idea what this is.
      need_help = true;
    break;
  }

  return !need_help;
}



// Prints a long help message.

void PrintLongHelp()
{
  PrintConstString(
"Commands:\r\n"
" ?, HLP  :  Help screen.\r\n"
"    QRY  :  Query system state.\r\n"
"  ECH 1/0:  Start/stop echoing typed characters back to the host.\r\n"
"    IDQ  :  Device identification string query.\r\n"
"    INI  :  Reinitialize (clock reset and events idled).\r\n"
"  LOG 1/0:  Start/stop reporting log data.\r\n"
"    LIN n:  Set the data reporting interval to n ticks.\r\n"
"  LTS 1/0:  Include/don't include timestamps in log reports.\r\n"
"    KIN n:  Set the nunchuck polling interval to n ticks.\r\n"
"  BLN 1/0:  Enable/disable status LED blinking.\r\n"
#if DEBUG_AVAILABLE
"Debugging commands:\r\n"
#endif
  );
}



// Reports the full system state. Verbose.
void QueryState()
{
  bool log_queued_copy, have_chuck_copy;

  // Copy volatile state information.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    log_queued_copy = log_queued;
    have_chuck_copy = have_chuck;
  }

  PrintConstString("System state (all values in base 10):\r\n");

  // Version information.
  PrintConstString("  Version:  " VERSION_STR "\r\n");

  // RTC information.

  PrintConstString("  Timestamp:  ");
  PrintUInt(QueryTimer());
  PrintConstString(" ticks\r\n");

  PrintConstString("  Clock ticks per second:  ");
  PrintUInt(RTC_TICKS_PER_SECOND);
  PrintConstString("\r\n");

  // Nunchuck state.

  if (have_chuck_copy)
    PrintConstString("  Nunchuck detected:  YES\r\n");
  else
    PrintConstString("  Nunchuck detected:  NO\r\n");

  PrintConstString("  Nunchuck polling interval:  ");
  PrintUInt(chuck_poll_period);
  PrintConstString(" ticks\r\n");

#if USE_SCRAMBLED_MODE
  PrintConstString("  Nunchuck data scrambling:  ON\r\n");
#else
  PrintConstString("  Nunchuck data scrambling:  OFF\r\n");
#endif

  // Diagnostics information.
  if (had_autostart)
    PrintConstString("  Automatic start:  YES\r\n");
  else
    PrintConstString("  Automatic start:  NO\r\n");

  // Logging information.

  if (log_queued_copy)
    PrintConstString("  Logging:  ON\r\n");
  else
    PrintConstString("  Logging:  OFF\r\n");

  PrintConstString("  Log interval:  ");
  PrintUInt(log_period);
  PrintConstString(" ticks\r\n");

  if (want_timestamp)
    PrintConstString("  Log timestamps:  ON\r\n");
  else
    PrintConstString("  Log timestamps:  OFF\r\n");

  // Blink code information.

  if (want_blink)
    PrintConstString("  Blink codes:  ON\r\n");
  else
    PrintConstString("  Blink codes:  OFF\r\n");

  PrintConstString("  Clock ticks per blink code bit:  ");
  PrintUInt(RTC_TICKS_PER_LED_TICK);
  PrintConstString("\r\n");

  // Done.
  PrintConstString("End of system state.\r\n");
}


//
// This is the end of the file.
