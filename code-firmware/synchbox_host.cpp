// Attention Circuits Control laboratory - I/O SynchBox project
// Host link routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes


#include "synchbox_includes.h"



//
// Global variables


// Configurable behavior parameters.
// These are high-level parameters; the event queue uses low-level timestamps.
// FIXME - These should probably be initialized via more-visible macros.
// This is especially true since ticks-per-second is defined elsewhere.

// FIXME - We can't get exactly 300 Hz. Going for slightly faster (303), to
// avoid missing eye-tracker samples. Beat frequency may be a problem. If so,
// change this to 25 (400 Hz) for a very fast beat frequency.
uint32_t log_period    = DEFAULT_LOGINTERVAL;

uint32_t timing_pulse_width      = 10;
uint32_t timing_pulse_period     = 10000;

uint32_t timing2_pulse_width      = 10;
uint32_t timing2_pulse_period     = 10000;

uint32_t evcode_config_setup    = 2;
uint32_t evcode_config_pulse    = 2;
uint32_t evcode_config_hold     = 2;
bool evcode_config_is_byte      = false;
bool evcode_config_want_strobe  = true;

verbosity_t verbosity = DEFAULT_VERBOSITY;

uint8_t filt_fuzz_lpf_bits = AN_FILTER_LPF_BITS_DEFAULT;
uint8_t filt_var_lpf_bits = AN_FILTER_SPIKE_LPF_BITS_DEFAULT;
uint8_t filt_spike_shift_bits = AN_FILTER_SPIKE_SIZE_BITS_DEFAULT;

bool debug_light_samples = false;

bool debug_trig_reward = false;
bool debug_trig_timing = false;
bool debug_trig_evcode = false;

bool debug_monitor_analog = false;
uint16_t debug_monitor_analog_channel = 0;



//
// Private variables


bool echo_active;



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
  echo_active = false;
  if (ECHO_DEFAULT)
    echo_active = true;

  InitSerialLink(CPU_SPEED, HOST_BAUD);

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
        }
      }
      else
        need_help = true;
    break;

    case 'L':
      // Log report management.
      // Command is "LIN n", "LVB n", or "LOG n".
      // These can all be updated at any time.
      // These are not ISR-updated.
      if (!argvalid)
        need_help = true;
      else if ('I' == opcode[1])
      {
        // FIXME - We should sanity-check this.
        log_period = argument;
      }
      else if ('V' == opcode[1])
      {
        verbosity = VERB_FULL;
        if (0 == argument)
          verbosity = VERB_PACKED;
        else if (1 == argument)
          verbosity = VERB_TERSE;
      }
      else if ('O' == opcode[1])
      {
        if (0 != argument)
        {
          next_log = QueryTimer();
          log_queued = true;
        }
        else
        {
          log_queued = false;
        }
      }
      else
        need_help = true;
    break;

    case 'T':
      // Timing pulse management.
      // Command is "TPW n", "TPP n", or "TIM n".
      // For channel 2, "TBW n", "TBP n", or "TIB n".
      // These can all be updated at any time.
      // These are accessed from the ISR.
      if (!argvalid)
        need_help = true;
      else if ('P' == opcode[1])
      {
        if ('W' == opcode[2])
        {
          // FIXME - We should sanity-check this.
          // The ISR reads this.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            timing_pulse_width = argument;
          }
        }
        else if ('P' == opcode[2])
        {
          // FIXME - We should sanity-check this.
          // The ISR reads this.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            timing_pulse_period = argument;
          }
        }
        else
          need_help = true;
      }
      else if ('B' == opcode[1])
      {
        if ('W' == opcode[2])
        {
          // FIXME - We should sanity-check this.
          // The ISR reads this.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            timing2_pulse_width = argument;
          }
        }
        else if ('P' == opcode[2])
        {
          // FIXME - We should sanity-check this.
          // The ISR reads this.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            timing2_pulse_period = argument;
          }
        }
        else
          need_help = true;
      }
      else if ('I' == opcode[1])
      {
        if ('M' == opcode[2])
        {
          // The ISR reads this.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            if (0 != argument)
            {
              next_timing_on = QueryTimer_ISR();
              next_timing_off = next_timing_on + timing_pulse_width;
              timing_queued = true;
            }
            else
            {
              timing_queued = false;
            }
          }
        }
        else if ('B' == opcode[2])
        {
          // The ISR reads this.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            if (0 != argument)
            {
              next_timing2_on = QueryTimer_ISR();
              next_timing2_off = next_timing2_on + timing2_pulse_width;
              timing2_queued = true;
            }
            else
            {
              timing2_queued = false;
            }
          }
        }
        else
          need_help = true;
      }
      else
        need_help = true;
    break;

    case 'R':
      // Reward pulse management.
      // There's only one command ("RWD n").
      // For channel 2, "RWB n".
      // These can only be updated if a reward pulse is not still in progress.
      // These are accessed from the ISR.
      if (!argvalid)
        need_help = true;
      else if ('D' == opcode[2])
      {
        // The ISR reads this.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // Only proceed if the previous reward has finished.
          if (!reward_queued)
          {
            // FIXME - We should sanity-check the argument for this.
            scratch32 = argument;
            next_reward_on = QueryTimer_ISR();
            next_reward_off = next_reward_on + scratch32;
            reward_queued = true;
          }
        }
      }
      else if ('B' == opcode[2])
      {
        // The ISR reads this.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // Only proceed if the previous reward has finished.
          if (!reward2_queued)
          {
            // FIXME - We should sanity-check the argument for this.
            scratch32 = argument;
            next_reward2_on = QueryTimer_ISR();
            next_reward2_off = next_reward2_on + scratch32;
            reward2_queued = true;
          }
        }
      }
      else
        need_help = true;
    break;

    case 'N':
      // Event code message management.
      // This is "NSE n", "NSU n", "NHD n", "NPD n", "NEU n", or "NDW n".
      // Configuration can be updated at any time, but an event can only be
      // generated if the previous event is not still in progress.
      // Some of these are accessed from the ISR.
      if (!argvalid)
        need_help = true;
      else if ('S' == opcode[1])
      {
        if ('E' == opcode[2])
        {
          // FIXME - The ISR reads this, but only for debug-triggered codes.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            if (0 == argument)
              evcode_config_want_strobe = false;
            else
              evcode_config_want_strobe = true;
          }
        }
        else if ('U' == opcode[2])
        {
          // FIXME - The ISR reads this, but only for debug-triggered codes.
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            // FIXME - We should sanity-check the argument for this.
            evcode_config_setup = argument;
          }
        }
        else
          need_help = true;
      }
      else if ('H' == opcode[1])
      {
        // FIXME - The ISR reads this, but only for debug-triggered codes.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // FIXME - We should sanity-check the argument for this.
          evcode_config_hold = argument;
        }
      }
      else if ('P' == opcode[1])
      {
        // FIXME - The ISR reads this, but only for debug-triggered codes.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // FIXME - We should sanity-check the argument for this.
          evcode_config_pulse = argument;
        }
      }
      else if ('E' == opcode[1])
      {
        // The ISR reads this.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // Only proceed if the previous event code has finished.
          if (!evcode_queued)
          {
            // NOTE - We're storing a 16-bit argument, but if we're using
            // strobing, the most significant bit is discarded.
            // FIXME - Starting in the current timeslice fails.
            // Schedule for the next timeslice instead.
            if (evcode_config_want_strobe)
            {
              evcode_data_on = QueryTimer_ISR() + 1;
              evcode_strobe_on = evcode_data_on + evcode_config_setup;
              evcode_strobe_off = evcode_strobe_on + evcode_config_pulse;
              evcode_data_off = evcode_strobe_off + evcode_config_hold;
            }
            else
            {
              // FIXME - Use a window length of 0 to signal "no strobe".
              evcode_data_on = QueryTimer_ISR() + 1;
              evcode_strobe_on = evcode_data_on;
              evcode_data_off = evcode_data_on + evcode_config_pulse;
              evcode_strobe_off = evcode_data_off;
            }

            if (evcode_config_is_byte)
            {
              // FIXME - Turn this into a 16-bit event.
              // The least-significant 8 bits stay zero always.
              evcode_data_value = (argument & 0xff) << 8;
            }
            else
              evcode_data_value = argument;

            evcode_queued = true;
          }
        }
      }
      else if ('D' == opcode[1])
      {
        // The ISR does not read this.
        if (8 == argument)
          evcode_config_is_byte = true;
        else if (16 == argument)
          evcode_config_is_byte = false;
        else
          need_help = true;
      }
      else
        need_help = true;
    break;

    case 'C':
      // Light sensor calibration management.
      // This is CSR, CSL, CSI, CAO n, CAF n, CTR n, or CTL n.
      // All of this may be accessed by the ISR, so lock it before changing.
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        if ('S' == opcode[1])
        {
          // We're choosing a calibration source.
          if ('R' == opcode[2])
          {
            light_cal_src = LIGHT_CAL_SRC_RIGHT;
          }
          else if ('L' == opcode[2])
          {
            light_cal_src = LIGHT_CAL_SRC_LEFT;
          }
          else if ('I' == opcode[2])
          {
            light_cal_src = LIGHT_CAL_SRC_INDEP;
          }
          else
            need_help = true;
        }
        else if ('A' == opcode[1])
        {
          if (!argvalid)
            need_help = true;
          else if ('O' == opcode[2])
          {
            LightCalInit_ISR(LIGHT_CAL_STATE_ONESHOT, argument);
          }
          else if ('F' == opcode[2])
          {
            LightCalInit_ISR(LIGHT_CAL_STATE_FREERUN, argument);
          }
          else
            need_help = true;
        }
        else if ('T' == opcode[1])
        {
          if (!argvalid)
            need_help = true;
          else if ('R' == opcode[2])
          {
            light_threshold_right = argument;
          }
          else if ('L' == opcode[2])
          {
            light_threshold_left = argument;
          }
          else
            need_help = true;
        }
      }
    break;

    case 'F':
      // Analog noise filter management.
      // This is FIL n, FST n, or FSW n.
      if (!argvalid)
        need_help = true;
      else
      {
        // Force sanity on the argument value.
        // FIXME - Arbitrarily limiting this to 2^8!
        if (argument > 8)
          argument = 8;

        // All of this may be accessed by the ISR, so lock it before changing.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          if ('I' == opcode[1])
            filt_fuzz_lpf_bits = (uint8_t) argument;
          else if ('S' == opcode[1])
          {
            if ('T' == opcode[2])
              filt_spike_shift_bits = argument;
            else if ('W' == opcode[2])
              filt_var_lpf_bits = argument;
            else
              need_help = true;
          }
          else
            need_help = true;

          if (!need_help)
          {
            // Update the filter parameters.
            ConfigureNoiseFilters(filt_fuzz_lpf_bits,
              filt_var_lpf_bits, filt_spike_shift_bits);
          }
        }
      }
    break;

#if DEBUG_AVAILABLE
    case 'X':
      // Debug ("examine" etc) functions.
      // This is XAL 1/0 or XJL 0/1, for now.
      if (!argvalid)
        need_help = true;
      else if ('A' == opcode[1])
      {
        // Turn analog light sensor tattling on or off.
        if (0 == argument)
          debug_light_samples = false;
        else
          debug_light_samples = true;
      }
      else if ('J' == opcode[1])
      {
        // Swap or un-swap joystick XY and light sensor LR inputs.
        if (0 == argument)
          SetOptJoySwapState(false);
        else
          SetOptJoySwapState(true);
      }
      else if ('T' == opcode[1])
      {
        // The trigger and monitor readouts are handled by the ISR, so
        // changes to these must be atomic.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // Event triggering based on external input.
          if ('R' == opcode[2])
            debug_trig_reward = ((0 == argument) ? false : true);
          else if ('T' == opcode[2])
            debug_trig_timing = ((0 == argument) ? false : true);
          else if ('N' == opcode[2])
            debug_trig_evcode = ((0 == argument) ? false : true);
          else
            need_help = true;
        }
      }
      else if ('M' == opcode[1])
      {
        // The trigger and monitor readouts are handled by the ISR, so
        // changes to these must be atomic.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          // Monitor output based on analog inputs.
          if ('D' == opcode[2])
            debug_monitor_analog = false;
          else if ('A' == opcode[2])
          {
            if (argument < DEBUG_ANALOG_CHANCOUNT)
            {
              debug_monitor_analog_channel = argument;
              debug_monitor_analog = true;
            }
            else
              need_help = true;
          }
          else
            need_help = true;
        }
      }
      else
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
"LVB 2/1/0:  Set data report verbosity (2 = full, 1 = terse, 0 = packed hex).\r\n"
"    TPW n:  Set the timing pulse duration to n ticks.\r\n"
"    TPP n:  Set the timing pulse period to n ticks.\r\n"
"  TIM 1/0:  Enable/disable timing pulses.\r\n"
"    TBW n:  Set the timing channel 2 pulse duration to n ticks.\r\n"
"    TBP n:  Set the timing channel 2 pulse period to n ticks.\r\n"
"  TIB 1/0:  Enable/disable timing channel 2 pulses.\r\n"
"    RWD n:  Send a reward pulse lasting n ticks.\r\n"
"    RWB n:  Send a reward channel 2 pulse lasting n ticks.\r\n"
"    NSU n:  Set the event code pre-strobe setup time to n ticks.\r\n"
"    NHD n:  Set the event code post-strobe hold time to n ticks.\r\n"
"  NSE 1/0:  Enable/disable strobing the most significant event code bit.\r\n"
"    NPD n:  Set the event code strobe pulse duration to n ticks.\r\n"
"    NEU n:  Emit value n over the event code parallel interface.\r\n"
"    NDW n:  Set event code data width to n bits (8 or 16).\r\n"
"CSL/R/I  :  Light sensor calibration slaved to left, right, or independent.\r\n"
"  CAO/F n:  Calibrate light sensors over n-tick window (one-shot/free-running).\r\n"
"  CTR/L n:  Force left/right light sensor threshold value to n.\r\n"
"    FIL n:  Set analog noise filter window to 2^n samples (0 disables).\r\n"
"    FST n:  Set analog spike rejection threshold to 2^n times the variance.\r\n"
"    FSW n:  Set spike rejection variance window to 2^n samples (0 disables).\r\n"
#if DEBUG_AVAILABLE
"Debugging commands:\r\n"
"  XAL 1/0:  Start/stop streaming full-rate analog light sample data.\r\n"
"  XJL 1/0:  Swaps or un-swaps joystick XY and light sensor LR inputs.\r\n"
"  XTR 1/0:  Enable/disable reward triggered by rising edge of GP00.\r\n"
"  XTT 1/0:  Enable/disable timing active on high level on GP00.\r\n"
"  XTN 1/0:  Enable/disable event code triggered by rising edge of GP00.\r\n"
"    XMA n:  Emit a rising edge on GP01 when analog 0..4 (X/Y/Z/L/R) rises.\r\n"
"    XMD  :  Disable GP01 analog diagnostic output.\r\n"
#endif
  );
}



// Reports the full system state. Verbose.
void QueryState()
{
  bool log_queued_copy, timing_queued_copy, timing2_queued_copy;
  uint8_t filt_fuzz_bits_copy, filt_var_bits_copy, filt_spike_bits_copy;
  uint16_t thresh_L_copy, thresh_R_copy;
  light_cal_src_t cal_src_copy;
  bool trig_reward_copy, trig_timing_copy, trig_evcode_copy;
  bool monitor_analog_copy;
  uint8_t monitor_channel_copy;

  // Copy volatile state information.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    log_queued_copy = log_queued;
    timing_queued_copy = timing_queued;
    timing2_queued_copy = timing2_queued;

    filt_fuzz_bits_copy = filt_fuzz_lpf_bits;
    filt_var_bits_copy = filt_var_lpf_bits;
    filt_spike_bits_copy = filt_spike_shift_bits;

    thresh_L_copy = light_threshold_left;
    thresh_R_copy = light_threshold_right;
    cal_src_copy = light_cal_src;

#if DEBUG_AVAILABLE
    trig_reward_copy = debug_trig_reward;
    trig_timing_copy = debug_trig_timing;
    trig_evcode_copy = debug_trig_evcode;
    monitor_analog_copy = debug_monitor_analog;
    monitor_channel_copy = debug_monitor_analog_channel;
#endif
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

  // I/O information.

  PrintConstString("  Reward I/O:  ");
  PrintConstString(REWARD_DESC);
  PrintConstString("    Reward ch2 I/O:  ");
  PrintConstString(REWARD2_DESC);
  PrintConstString("\r\n  Timing I/O:  ");
  PrintConstString(TIMING_DESC);
  PrintConstString("    Timing ch2 I/O:  ");
  PrintConstString(TIMING2_DESC);
#if DEBUG_AVAILABLE
  PrintConstString("\r\n  Debug trigger input:  ");
  PrintConstString(DEBUGTRIG_DESC);
  PrintConstString("\r\n  Debug monitor output:  ");
  PrintConstString(DEBUGMON_DESC);
#endif
  PrintConstString("\r\n");

  // Logging information.

  if (log_queued_copy)
    PrintConstString("  Logging:  ON\r\n");
  else
    PrintConstString("  Logging:  OFF\r\n");

  PrintConstString("  Log interval:  ");
  PrintUInt(log_period);
  PrintConstString(" ticks\r\n");

  switch (verbosity)
  {
    case VERB_PACKED: PrintConstString("  Verbosity:  Packed hex\r\n"); break;
    case VERB_TERSE:  PrintConstString("  Verbosity:  Terse\r\n"); break;
    default:          PrintConstString("  Verbosity:  Full\r\n"); break;
  };

  // Timing pulse information.

  if (timing_queued_copy)
    PrintConstString("  Timing:  ON\r\n");
  else
    PrintConstString("  Timing:  OFF\r\n");

  PrintConstString("  Timing pulse width/period:  ");
  PrintUInt(timing_pulse_width);
  PrintConstString(" / ");
  PrintUInt(timing_pulse_period);
  PrintConstString(" ticks\r\n");

  if (timing2_queued_copy)
    PrintConstString("  Timing ch2:  ON\r\n");
  else
    PrintConstString("  Timing ch2:  OFF\r\n");

  PrintConstString("  Timing ch2 pulse width/period:  ");
  PrintUInt(timing2_pulse_width);
  PrintConstString(" / ");
  PrintUInt(timing2_pulse_period);
  PrintConstString(" ticks\r\n");

  // Event code information.

  PrintConstString("  Event code data front-porch/strobe/back-porch:  ");
  PrintUInt(evcode_config_setup);
  PrintConstString(" / ");
  PrintUInt(evcode_config_pulse);
  PrintConstString(" / ");
  PrintUInt(evcode_config_hold);
  PrintConstString(" ticks\r\n");

  if (evcode_config_want_strobe)
    PrintConstString("  Event code strobe:  ON\r\n");
  else
    PrintConstString("  Event code strobe:  OFF\r\n");

  if (evcode_config_is_byte)
    PrintConstString("  Event code data width:   8 bits\r\n");
  else
    PrintConstString("  Event code data width:  16 bits\r\n");

  // Light sensor information.

  PrintConstString("  Light sensor thresholds (left/right):  ");
  PrintUInt(thresh_L_copy);
  PrintConstString(" / ");
  PrintUInt(thresh_R_copy);
  PrintConstString("\r\n");

  if (LIGHT_CAL_SRC_LEFT == cal_src_copy)
    PrintConstString("  Light threshold slaving:  LEFT\r\n");
  else if (LIGHT_CAL_SRC_RIGHT == cal_src_copy)
    PrintConstString("  Light threshold slaving:  RIGHT\r\n");
  else
    PrintConstString("  Light threshold slaving:  INDEPENDENT\r\n");

  // FIXME - ADC "swapped joystick and light sensor" flag.
  // This is a workaround for filter hardware issues.
  if (light_joystick_swapped)
    PrintConstString("  Light sensor L/R are SWAPPED with joystick X/Y.\r\n");

  // Noise filtering (light sensors and joysticks).
  // Add analog word width here too.

  if (ANALOG_16BIT)
    PrintConstString("  Analog sample size:  16 bits\r\n");
  else
    PrintConstString("  Analog sample size:   8 bits\r\n");

  PrintConstString("  Noise suppression filter:  ");
  if (0 == filt_fuzz_bits_copy)
    PrintConstString("OFF\r\n");
  else
  {
    PrintUInt(filt_fuzz_bits_copy);
    PrintConstString(" bits  (decay length ");
    PrintUInt(1 << filt_fuzz_bits_copy);
    PrintConstString(" samples)\r\n");
  }

  PrintConstString("  Spike suppression filter:");
  if (0 == filt_var_bits_copy)
    PrintConstString("  OFF\r\n");
  else
  {
    PrintConstString("\r\n  Variance LPF:  ");
    PrintUInt(filt_var_bits_copy);
    PrintConstString(" bits  (decay length ");
    PrintUInt(1 << filt_var_bits_copy);
    PrintConstString(" samples)\r\n");
    PrintConstString("  Spike rejection threshold:  2^");
    PrintUInt(filt_spike_bits_copy);
    PrintConstString(" (");
    PrintUInt(1 << filt_spike_bits_copy);
    PrintConstString("x)\r\n");
  }

#if DEBUG_AVAILABLE
  PrintConstString("  External event triggers:");
  if (trig_reward_copy) PrintConstString("  RWD");
  if (trig_timing_copy) PrintConstString("  TIM");
  if (trig_evcode_copy) PrintConstString("  NEU");
  if (!( trig_reward_copy || trig_timing_copy || trig_evcode_copy ))
    PrintConstString("  none");
  PrintConstString("\r\n");

  PrintConstString("  Analog input being monitored:  ");
  if (monitor_analog_copy)
  {
    if (monitor_channel_copy < DEBUG_ANALOG_CHANCOUNT)
    {
      PrintChar(debug_monitor_analog_labels[monitor_channel_copy]);
      PrintConstString("\r\n");
    }
    else
      PrintConstString("BOGUS\r\n");
  }
  else
    PrintConstString("none\r\n");

  PrintConstString("  Joystick analog threshold:  ");
  PrintUInt(debug_threshold_middle_uint);
  PrintConstString("\r\n");
#endif

  // Done.
  PrintConstString("End of system state.\r\n");
}


//
// This is the end of the file.
