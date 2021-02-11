// Attention Circuits Control laboratory - I/O SynchBox project
// Event queue
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "synchbox_includes.h"


//
// Global variables

// ADC round-robin rollover flag.
// This is volatile, because the ISR updates it.
volatile bool adc_new_round;

// FIXME - ADC "swapped joystick and light sensor" flag.
// This is advisory, set when the swap state changes.
// This is a workaround for filter hardware issues.
bool light_joystick_swapped = false;

// Event queue.
// Times are in ticks.
// These are volatile, because ISRs can queue and/or handle some of them.

// Log message event.
// These are not ISR-updated.
bool log_queued;
uint32_t next_log;
// No state tracking needed, as this is an instantaneous event.

// Reward pulse event.

volatile bool reward_queued;
volatile uint32_t next_reward_on;
volatile uint32_t next_reward_off;

volatile bool reward2_queued;
volatile uint32_t next_reward2_on;
volatile uint32_t next_reward2_off;

// Timing pulse event.

volatile bool timing_queued;
volatile uint32_t next_timing_on;
volatile uint32_t next_timing_off;

volatile bool timing2_queued;
volatile uint32_t next_timing2_on;
volatile uint32_t next_timing2_off;

// Event code event.
volatile bool evcode_queued;
volatile uint32_t evcode_data_on;
volatile uint32_t evcode_data_off;
volatile uint32_t evcode_strobe_on;
volatile uint32_t evcode_strobe_off;
volatile uint16_t evcode_data_value;



//
// Private enums

enum reward_state_t
{
  REWARD_LOW,
  REWARD_HIGH
};

enum timing_state_t
{
  TIMING_LOW,
  TIMING_HIGH
};

enum evcode_state_t
{
  ECSTATE_OFF,
  ECSTATE_FP,
  ECSTATE_STROBE,
  ECSTATE_BP
};



//
// Private macros

// Wrapper for printing analog samples.
#if ANALOG_16BIT
#define ANALOGPRINT(X) PrintHex16(X)
#else
#define ANALOGPRINT(X) PrintHex8(X)
#endif



//
// Private variables

// ISR mutex.
volatile bool in_isr;

// Event state and bookkeeping information.
// Times are in ticks.

// Log message event.
// No state tracking needed, as this is an instantaneous event.

// Reward pulse event.

// State tracking.
volatile reward_state_t reward_state;
volatile reward_state_t reward2_state;

// Logging record for this event.

volatile bool reward_sent;
volatile uint32_t reward_sent_time;
volatile uint16_t reward_sent_length;

volatile bool reward2_sent;
volatile uint32_t reward2_sent_time;
volatile uint16_t reward2_sent_length;

// Timing pulse event.

volatile timing_state_t timing_state;
volatile timing_state_t timing2_state;

// Logging record for this event.

volatile bool timing_sent;
volatile uint32_t timing_sent_time;

volatile bool timing2_sent;
volatile uint32_t timing2_sent_time;

// Event code event.
volatile evcode_state_t evcode_state;
// Logging record for this event.
volatile bool evcode_sent;
volatile uint32_t evcode_sent_time;
volatile uint16_t evcode_sent_data;

// Debugging I/O.
// FIXME - Using static initialization for debug state.
// FIXME - These might not have to be volatile, since only the ISR uses them?
volatile bool debug_prev_trig_state = false;
volatile uint16_t debug_trig_timeout = 0;
volatile bool debug_prev_mon_state = false;
volatile analog_sample_t debug_prev_mon_data = 0;
volatile uint16_t debug_mon_timeout = 0;

// Analog input storage variables.
// FIXME - We track when these are read, but we don't report it.

volatile analog_sample_t adata_joy_x, adata_joy_y, adata_joy_c;
volatile uint32_t atime_joy_x, atime_joy_y, atime_joy_c;

volatile analog_sample_t adata_opt_left, adata_opt_right;
volatile uint32_t atime_opt_left, atime_opt_right;

// Analog noise filters (may or may not be enabled).

NoiseFilter nfilt_joy_x, nfilt_joy_y, nfilt_joy_c;
NoiseFilter nfilt_opt_left, nfilt_opt_right;

// Analog input list entry number.
// This tracks which input the ADC is reading now.
volatile int analog_input_idx;

// Debug channel mapping for analog inputs.
// FIXME - This is a constant, not a variable, but it's more readable here.
volatile analog_sample_t *debug_monitor_analog_data[DEBUG_ANALOG_CHANCOUNT] =
{ &adata_joy_x, &adata_joy_y, &adata_joy_c,
  &adata_opt_left, &adata_opt_right };

// Debug channel thresholds.
// FIXME - This has to be uint16_t, as light thresholds are hardcoded to that.
const analog_sample_t debug_analog_max = ~((analog_sample_t) 0);
//const analog_sample_t debug_threshold_middle = (~((analog_sample_t) 0)) >> 1;
//uint16_t debug_threshold_middle_uint = debug_threshold_middle;
uint16_t debug_threshold_middle_uint = ((uint16_t) debug_analog_max) >> 1;
volatile uint16_t
  *debug_monitor_analog_threshold[DEBUG_ANALOG_CHANCOUNT] =
{ &debug_threshold_middle_uint, &debug_threshold_middle_uint,
  &debug_threshold_middle_uint,
  &light_threshold_left, &light_threshold_right };

// ADC channel labels.
// FIXME - This is public, not private, but putting it here for readability.
char debug_monitor_analog_labels[DEBUG_ANALOG_CHANCOUNT] =
{ 'X', 'Y', 'Z', 'L', 'R' };



//
// Private constants

// The list of analog inputs to read.
// This associates ADC input pins to storage variables (and timestamps).
// Terminated by -1 and NULL values.

// FIXME - Making two versions, as a workaround for filter hardware issues.


struct analog_input_lut_row_t
{
  int pin_id;
  volatile analog_sample_t *data_p;
  volatile uint32_t *timestamp_p;
  NoiseFilter *nfilt_p;
};

const struct analog_input_lut_row_t analog_input_lut_normal[] =
{
  { JOY_X_PIN, &adata_joy_x, &atime_joy_x, &nfilt_joy_x },
  { JOY_Y_PIN, &adata_joy_y, &atime_joy_y, &nfilt_joy_y },
  { JOY_C_PIN, &adata_joy_c, &atime_joy_c, &nfilt_joy_c },
  { OPT_L_PIN, &adata_opt_left, &atime_opt_left, &nfilt_opt_left },
  { OPT_R_PIN, &adata_opt_right, &atime_opt_right, &nfilt_opt_right },
  { -1, NULL, NULL, NULL }
};

const struct analog_input_lut_row_t analog_input_lut_swapped[] =
{
  // Read light sensor input from the joystick ports.
  // To keep ADC timing the same, store light sensor port output (ground) joyXY.
  { OPT_L_PIN, &adata_joy_x, &atime_joy_x, &nfilt_joy_x },
  { OPT_R_PIN, &adata_joy_y, &atime_joy_y, &nfilt_joy_y },
  { JOY_C_PIN, &adata_joy_c, &atime_joy_c, &nfilt_joy_c },
  { JOY_X_PIN, &adata_opt_left, &atime_opt_left, &nfilt_opt_left },
  { JOY_Y_PIN, &adata_opt_right, &atime_opt_right, &nfilt_opt_right },
  { -1, NULL, NULL, NULL }
};

// Pointer to the appropriate table.
// Initialize this to a sane value on instantiation.
const analog_input_lut_row_t *analog_input_lut = analog_input_lut_normal;



//
// Functions


// Initializes the event queue.

void InitEventQueue(void)
{
  // ISRs may modify these, so we need to force this to be atomic.

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // ISR mutex, for nested interrupts.
    in_isr = false;


    // Log messages.

    log_queued = false;
    next_log = 0;


    // Reward pulse.

    reward_queued = false;
    next_reward_on = 0;
    next_reward_off = 0;

    reward_state = REWARD_LOW;

    reward_sent = false;
    reward_sent_time = 0;
    reward_sent_length = 0;


    reward2_queued = false;
    next_reward2_on = 0;
    next_reward2_off = 0;

    reward2_state = REWARD_LOW;

    reward2_sent = false;
    reward2_sent_time = 0;
    reward2_sent_length = 0;


    // Timing pulse.

    timing_queued = false;
    next_timing_on = 0;
    // Initialize to a correct value. A "timing on" command resets this.
    next_timing_off = next_timing_on + timing_pulse_width;

    timing_state = TIMING_LOW;

    timing_sent = false;
    timing_sent_time = 0;


    timing2_queued = false;
    next_timing2_on = 0;
    // Initialize to a correct value. A "timing on" command resets this.
    next_timing2_off = next_timing2_on + timing2_pulse_width;

    timing2_state = TIMING_LOW;

    timing2_sent = false;
    timing2_sent_time = 0;


    // Status LED.

    // FIXME - Blink codes aren't yet implemented.
    // Nothing to initialize.


    // Event code handshaking.

    evcode_queued = false;
    evcode_data_on = 0;
    evcode_data_off = 0;
    evcode_strobe_on = 0;
    evcode_strobe_off = 0;
    evcode_data_value = 0;

    evcode_state = ECSTATE_OFF;

    evcode_sent = false;
    evcode_sent_time = 0;
    evcode_sent_data = 0;


    // FIXME - Counting on static initialization for trigger/monitor state.


    // Analog inputs.

    adata_joy_x = 0;
    adata_joy_y = 0;
    adata_joy_c = 0;

    atime_joy_x = 0;
    atime_joy_y = 0;
    atime_joy_c = 0;

    adata_opt_left = 0;
    adata_opt_right = 0;

    atime_opt_left = 0;
    atime_opt_right = 0;

    analog_input_idx = -1;

    // Light sensor calibration.
    LightCalFirstInit();
    LightCalInit_ISR(LIGHT_CAL_STATE_DONE, 0);
  }
}



// Initializes pins that events use.

void InitEventPins(void)
{
  // Set pin states.
  // NOTE - This is done by register-twiddling, not the Arduino way.

  // Set data-direction bits.
  // Doing this via read-modify-write is acceptable.
  REWARD_DIRECTION |= REWARD_BIT;
  REWARD2_DIRECTION |= REWARD2_BIT;
  TIMING_DIRECTION |= TIMING_BIT;
  TIMING2_DIRECTION |= TIMING2_BIT;
  LED_DIRECTION |= LED_BIT;

  // Set initial output low.
  // Doing this via read-modify-write is acceptable.
  REWARD_PORT &= ~REWARD_BIT;
  REWARD2_PORT &= ~REWARD2_BIT;
  TIMING_PORT &= ~TIMING_BIT;
  TIMING2_PORT &= ~TIMING2_BIT;
  // FIXME - Just turn the status LED on and leave it.
  // Blink codes aren't yet implemented.
  LED_PORT |= LED_BIT;

  // FIXME - Event code stuff is done the hacky-way, and is m2560-specific.
  EVCODE_PORT_H = 0;
  EVCODE_PORT_L = 0;
  EVCODE_DDR_H = 0xff;
  EVCODE_DDR_L = 0xff;

#if DEBUG_AVAILABLE
  // Initialize GPIO debugging pins.
  // Doing this via read-modify-write is acceptable.

  DEBUGTRIG_DIRECTION &= ~DEBUGTRIG_BIT; // This is an input; direction 0.
  DEBUGTRIG_PORT |= DEBUGTRIG_BIT; // Enable pull-up, just in case.

  DEBUGMON_DIRECTION |= DEBUGMON_BIT;
  DEBUGMON_PORT &= ~DEBUGMON_BIT; // Set initial output low.
#endif

  // Nothing to do for the analog pins; InitADC() handled that.

  // We do need to record whether joystic and light sensor have been swapped.
  // FIXME - This is a workaround for filter hardware issues.
  SetOptJoySwapState(false);
}



// Handles queued digital I/O. This should be fast (interrupt-friendly).
// FIXME - This may still take a while to run.
// This works with a timer tick of 10 kHz, but isn't guaranteed above that.

void PollEventsISR(void)
{
  uint32_t thistime;
  bool analog_available;
  volatile analog_sample_t *analog_data_p;
  volatile uint32_t *analog_timestamp_p;
  NoiseFilter *nfilt_p;
  bool debug_this_trig;
  analog_sample_t debug_this_data, debug_this_threshold;
  bool debug_this_mon_raw;

  // We need to be able to service UART interrupts while the bulk of this
  // is running.
  // Using a wrapper so that if we get multiple calls from the timer interrupt
  // we only service the first one.


  // Events that are in progress always complete.
  // The "queued" flag is checked when initiating new events.


  // Some of this is atomic, and some of this isn't.
  // We know that this routine won't interrupt itself, and non-ISR code
  // won't be running, but other ISRs might interrupt it.
  // This means we need to atomic-block anything that an ISR might modify,
  // but otherwise don't have to worry.

  // NOTE - Right now the ADC is polling-based, so nothing we query should
  // be ISR-updated.


  if (!in_isr)
  {
    in_isr = true;

    // Get a timestamp, to avoid repeatedly checking it.
    thistime = QueryTimer_ISR();


#if ALLOW_NESTED_INTERRUPTS
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
#endif
    {


      // Check for debugging triggers and queue events if detected.
#if DEBUG_AVAILABLE

      // Check trigger state.
      debug_this_trig = false;
      if (DEBUGTRIG_INPUT & DEBUGTRIG_BIT)
        debug_this_trig = true;

      // Reward is positive edge triggered.
      // So is event code output.
      if ( debug_this_trig && (!debug_prev_trig_state)
        && (0 == debug_trig_timeout) )
      {
        // Start the timeout counting from this edge no matter what.
        debug_trig_timeout = DEBUG_DEADTIME;

        // Reward pulse.
        if (debug_trig_reward && (!reward_queued))
        {
          next_reward_on = thistime;
          // FIXME - No record of previous durations, so pick one by fiat.
          next_reward_off = next_reward_on + DEBUG_REWARDLENGTH;
          reward_queued = true;
        }

        // Event code output.
        if (debug_trig_evcode && (!evcode_queued))
        {
          // FIXME - Cut-and-pasted from _host.cpp.
          // A function would be too slow and a macro just as ugly.
          if (evcode_config_want_strobe)
          {
            evcode_data_on = thistime + 1;
            evcode_strobe_on = evcode_data_on + evcode_config_setup;
            evcode_strobe_off = evcode_strobe_on + evcode_config_pulse;
            evcode_data_off = evcode_strobe_off + evcode_config_hold;
          }
          else
          {
            // FIXME - Use a window length of 0 to signal "no strobe".
            evcode_data_on = thistime + 1;
            evcode_strobe_on = evcode_data_on;
            evcode_data_off = evcode_data_on + evcode_config_pulse;
            evcode_strobe_off = evcode_data_off;
          }

          // Use the last saved data value.
          evcode_queued = true;
        }
      }

      // Timing pulses are level-triggered.
      // Only turn _off_ if we see a zero after the dead time expires.
      if (debug_trig_timing)
      {
        if ( debug_this_trig || (0 < debug_trig_timeout) )
        {
          if (!timing_queued)
          {
            next_timing_on = thistime;
            next_timing_off = next_timing_on + timing_pulse_width;
            timing_queued = true;
          }
        }
        else
          timing_queued = false;
      }

      // Decrement the timeout if it's nonzero.
      if (0 < debug_trig_timeout)
        debug_trig_timeout--;

      // Record the previous state if we're not in dead time.
      if (0 == debug_trig_timeout)
        debug_prev_trig_state = debug_this_trig;

#endif


      // Reward pulse.

      // First channel.
      switch (reward_state)
      {
        case REWARD_HIGH:
          if (thistime >= next_reward_off)
          {
            // End of this reward pulse.
            REWARD_PORT &= ~REWARD_BIT;
            reward_state = REWARD_LOW;

            // Dequeue.
            reward_queued = false;
          }
        break;

        default: // REWARD_LOW
          if (reward_queued && (thistime >= next_reward_on))
          {
            // Start of this reward pulse.
            REWARD_PORT |= REWARD_BIT;
            reward_state = REWARD_HIGH;

            // Make note that we're sending a pulse.
            reward_sent = true;
            // Report nominal time rather than actual time.
            // These should match, unless we're dropping interrupts.
            reward_sent_time = next_reward_on;
            reward_sent_length = next_reward_off - next_reward_on;
          }
        break;
      }

      // Second channel.
      switch (reward2_state)
      {
        case REWARD_HIGH:
          if (thistime >= next_reward2_off)
          {
            // End of this reward pulse.
            REWARD2_PORT &= ~REWARD2_BIT;
            reward2_state = REWARD_LOW;

            // Dequeue.
            reward2_queued = false;
          }
        break;

        default: // REWARD_LOW
          if (reward2_queued && (thistime >= next_reward2_on))
          {
            // Start of this reward pulse.
            REWARD2_PORT |= REWARD2_BIT;
            reward2_state = REWARD_HIGH;

            // Make note that we're sending a pulse.
            reward2_sent = true;
            // Report nominal time rather than actual time.
            // These should match, unless we're dropping interrupts.
            reward2_sent_time = next_reward2_on;
            reward2_sent_length = next_reward2_off - next_reward2_on;
          }
        break;
      }


      // Timing pulse.

      // First channel.
      switch (timing_state)
      {
        case TIMING_HIGH:
          if (thistime >= next_timing_off)
          {
            // End of this timing pulse.
            TIMING_PORT &= ~TIMING_BIT;
            timing_state = TIMING_LOW;

            // Re-enqueue; the fact that we're here means timing is enabled.
            next_timing_on += timing_pulse_period;
            // Pulse width may have changed; recompute the "off" time.
            next_timing_off = next_timing_on + timing_pulse_width;
          }
        break;

        default: // TIMING_LOW
          if (timing_queued && (thistime >= next_timing_on))
          {
            // Start of this timing pulse.
            TIMING_PORT |= TIMING_BIT;
            timing_state = TIMING_HIGH;

            // Make note that we're sending a pulse.
            timing_sent = true;
            // Report nominal time rather than actual time.
            // These should match, unless we're dropping interrupts.
            timing_sent_time = next_timing_on;
          }
        break;
      }

      // Second channel.
      switch (timing2_state)
      {
        case TIMING_HIGH:
          if (thistime >= next_timing2_off)
          {
            // End of this timing pulse.
            TIMING2_PORT &= ~TIMING2_BIT;
            timing2_state = TIMING_LOW;

            // Re-enqueue; the fact that we're here means timing is enabled.
            next_timing2_on += timing2_pulse_period;
            // Pulse width may have changed; recompute the "off" time.
            next_timing2_off = next_timing2_on + timing2_pulse_width;
          }
        break;

        default: // TIMING_LOW
          if (timing2_queued && (thistime >= next_timing2_on))
          {
            // Start of this timing pulse.
            TIMING2_PORT |= TIMING2_BIT;
            timing2_state = TIMING_HIGH;

            // Make note that we're sending a pulse.
            timing2_sent = true;
            // Report nominal time rather than actual time.
            // These should match, unless we're dropping interrupts.
            timing2_sent_time = next_timing2_on;
          }
        break;
      }


      // Event code handshaking.
      // FIXME - Event code stuff is done the hacky-way, and is m2560-specific.
      // FIXME - The user sees 8- or 16-bit words, but the implementation is
      // 16-bit. The least significant 8 bits stay 0 for 8-bit mode.
      // FIXME - For "no strobe", the strobe and data on/off times are the
      // same. Explicitly special-case this.

      switch (evcode_state)
      {
        case ECSTATE_BP:
          if (thistime >= evcode_data_off)
          {
            EVCODE_PORT_H = 0x00;
            EVCODE_PORT_L = 0x00;

            evcode_state = ECSTATE_OFF;

            // Dequeue.
            evcode_queued = false;
          }
        break;

        case ECSTATE_STROBE:
          if (thistime >= evcode_data_off)
          {
            // We're not strobing; go straight to "off".
            EVCODE_PORT_H = 0x00;
            EVCODE_PORT_L = 0x00;

            evcode_state = ECSTATE_OFF;

            // Dequeue.
            evcode_queued = false;
          }
          else if (thistime >= evcode_strobe_off)
          {
            evcode_data_value &= 0x7fff;
            EVCODE_PORT_H = (uint8_t) ((evcode_data_value >> 8) & 0xff);
            EVCODE_PORT_L = (uint8_t) (evcode_data_value & 0xff);

            evcode_state = ECSTATE_BP;
          }
        break;

        case ECSTATE_FP:
          if (thistime >= evcode_strobe_on)
          {
            evcode_data_value |= 0x8000;
            EVCODE_PORT_H = (uint8_t) ((evcode_data_value >> 8) & 0xff);
            EVCODE_PORT_L = (uint8_t) (evcode_data_value & 0xff);

            evcode_state = ECSTATE_STROBE;

            // Make note that we're sending a data word.
            evcode_sent = true;
            // Report nominal time rather than actual time.
            // These should match, unless we're dropping interrupts.
            evcode_sent_time = evcode_strobe_on;
            evcode_sent_data = evcode_data_value & 0x7fff;
          }
        break;

        default: // ECSTATE_OFF
          if (evcode_queued && (thistime >= evcode_strobe_on))
          {
            // We're not strobing; assert data and skip "front porch".
            EVCODE_PORT_H = (uint8_t) ((evcode_data_value >> 8) & 0xff);
            EVCODE_PORT_L = (uint8_t) (evcode_data_value & 0xff);

            evcode_state = ECSTATE_STROBE;

            // Make note that we're sending a data word.
            evcode_sent = true;
            // Report nominal time rather than actual time.
            // These should match, unless we're dropping interrupts.
            evcode_sent_time = evcode_strobe_on;
            // We're using the full 16-bit value.
            evcode_sent_data = evcode_data_value;
          }
          else if (evcode_queued && (thistime >= evcode_data_on))
          {
            evcode_data_value &= 0x7fff;
            EVCODE_PORT_H = (uint8_t) ((evcode_data_value >> 8) & 0xff);
            EVCODE_PORT_L = (uint8_t) (evcode_data_value & 0xff);

            evcode_state = ECSTATE_FP;
          }
        break;
      }


      // Analog input.
      // We read one input at a time, initiating when available and
      // recording completed cycles' data.

      // Make this atomic, just in case there's a UART delay mid-update.
#if ALLOW_NESTED_INTERRUPTS
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#endif
      {
        analog_available = PollADC_ISR();
      }

      if (analog_available)
      {
        // An ADC transaction completed (or this is our first call).

        // Save the data, if it's valid.
        // Increment the index once we've done so.
        if (0 <= analog_input_idx)
        {
          // We should be pointing at a valid LUT entry.

          analog_data_p = analog_input_lut[analog_input_idx].data_p;
          analog_timestamp_p = analog_input_lut[analog_input_idx].timestamp_p;
          nfilt_p = analog_input_lut[analog_input_idx].nfilt_p;

          if ( (NULL != analog_data_p) && (NULL != analog_timestamp_p)
            && (NULL != nfilt_p) )
          {
            if (adc_data_valid)
            {
              // FIXME - An object call for this is almost certainly slow.
              // No clean way to change it.
              // The filter routine is always called, but doesn't
              // necessarily change the data.
              adc_data = nfilt_p->FilterSample(adc_data);

              (*analog_data_p) = adc_data;
              (*analog_timestamp_p) = thistime;

              // Tell the light sensor calibration routine about this change.
              // FIXME - A function call for this may be slow?
              LightCalUpdate_ISR(analog_input_lut[analog_input_idx].pin_id,
                adc_data, thistime);
            }
          }

          // Move to the next LUT entry. Wrap around if we hit EOL.
          analog_input_idx++;
          // This _will_ contain invalid data if we're at EOL.
          if ( (0 > analog_input_lut[analog_input_idx].pin_id)
            && (NULL == analog_input_lut[analog_input_idx].data_p)
            && (NULL == analog_input_lut[analog_input_idx].timestamp_p) )
          {
            // Wrap around.
            analog_input_idx = 0;
            adc_new_round = true;
          }
        }
        else
        {
          // First-time initialization.
          analog_input_idx = 0;
          adc_new_round = false;
        }

        // Queue a new read.
        // If we're pointing at a bogus entry, do nothing.
        // That shouldn't happen, so nothing is the right thing to do.
        if ( (0 <= analog_input_lut[analog_input_idx].pin_id)
          && (NULL != analog_input_lut[analog_input_idx].data_p)
          && (NULL != analog_input_lut[analog_input_idx].timestamp_p)
          && (NULL != analog_input_lut[analog_input_idx].nfilt_p) )
        {
          // Make this atomic, just in case there's a UART delay mid-update.
#if ALLOW_NESTED_INTERRUPTS
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#endif
          {
            StartAnalogRead_ISR(analog_input_lut[analog_input_idx].pin_id);
          }
        }
      }


      // Check for changes to the analog channel we're monitoring.
#if DEBUG_AVAILABLE

      if ( debug_monitor_analog && (0 == debug_mon_timeout) )
      {
        // Sanity check.
        if (DEBUG_ANALOG_CHANCOUNT <= debug_monitor_analog_channel)
          debug_monitor_analog_channel = 0;

        // Fetch raw data and threshold value.
        debug_this_data =
          *(debug_monitor_analog_data[debug_monitor_analog_channel]);

        debug_this_threshold = (analog_sample_t)
          (*(debug_monitor_analog_threshold[debug_monitor_analog_channel]));

        // Update our output state no matter what.
        // This ends up being a debounced buffer of the input, which is ok.
        // All we guarantee is the rising edge, but this gives falling too.
        if (debug_this_data >= debug_this_threshold)
          DEBUGMON_PORT |= DEBUGMON_BIT;
        else
          DEBUGMON_PORT &= ~DEBUGMON_BIT;
      }

      // Decrement the timeout if it's nonzero.
      if (0 < debug_mon_timeout)
        debug_mon_timeout--;

      // Record the previous state if we're not in dead time.
      // FIXME - Not _using_ previous state information.
      if (0 == debug_mon_timeout)
        debug_prev_mon_data = debug_this_data;

#endif


      // Done.

      // Nested ISR wrapper ends.
    }

    in_isr = false;
  }
}



// Handles event reporting. This is slow (interrupt-unfriendly).

void PollEventReporting(void)
{
  uint32_t thistime;
  bool need_report;
  bool had_reward;
  uint32_t reward_time;
  uint16_t reward_length;
  bool had_reward2;
  uint32_t reward2_time;
  uint16_t reward2_length;
  bool had_timing;
  uint32_t timing_time;
  bool had_timing2;
  uint32_t timing2_time;
  bool had_evcode;
  uint32_t evcode_time_copy;
  uint16_t evcode_data_copy;
  analog_sample_t joy_X, joy_Y, joy_C;
  analog_sample_t opt_L, opt_R;
  bool analog_new_round;
  uint16_t opt_thresh_L, opt_thresh_R;
  light_cal_src_t opt_thresh_src;
  bool opt_left, opt_right;


  // Check to see what's updated.
  // The ISR can update these variables, so make copies.

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // Record the timestamp.
    thistime = QueryTimer_ISR();


    // Logging.
    need_report = false;
    if (log_queued)
    {
      // NOTE - Only iterate once _within_ the atomic block.
      // We do need to soak up any potential long delays later.
      // FIXME - This causes us to firehose if next_log has wrapped
      // around but thistime hasn't.
      if (thistime >= next_log)
      {
        need_report = true;
        next_log += log_period;
      }
    }

    // Reward pulse.

    had_reward = reward_sent;
    reward_time = reward_sent_time;
    reward_length = reward_sent_length;
    reward_sent = false;

    had_reward2 = reward2_sent;
    reward2_time = reward2_sent_time;
    reward2_length = reward2_sent_length;
    reward2_sent = false;

    // Timing pulse.

    had_timing = timing_sent;
    timing_time = timing_sent_time;
    timing_sent = false;

    had_timing2 = timing2_sent;
    timing2_time = timing2_sent_time;
    timing2_sent = false;

    // Event code handshaking.
    had_evcode = evcode_sent;
    evcode_time_copy = evcode_sent_time;
    evcode_data_copy = evcode_sent_data;
    // Translate this back to the user's formatting convention.
    if (evcode_config_is_byte)
      evcode_data_copy >>= 8;
    evcode_sent = false;

    // Analog inputs.
    // FIXME - Discarding timestamps for these!
    // We're pretending they happened at this sampling point, but there
    // could be a skew of up to 5 times the ADC sample time.

    joy_X = adata_joy_x;
    joy_Y = adata_joy_y;
    joy_C = adata_joy_c;

    analog_new_round = adc_new_round;
    // Reset the new-round flag no matter what.
    adc_new_round = false;

    // Pull in all of the thresholding information for the light sensors too.
    opt_L = adata_opt_left;
    opt_R = adata_opt_right;
    opt_thresh_L = light_threshold_left;
    opt_thresh_R = light_threshold_right;
    opt_thresh_src = light_cal_src;
  }


  // Now that we're outside the atomic block, soak up any long delay
  // that accumulated with the report.
  if (log_queued)
  {
    // FIXME - Handle the wrap-around case, just in case someone leaves
    // this running for 120 hours.
    // Also handle the zero case, instead of spinning forever.
    while ( (0 < log_period) && (thistime >= next_log)
      && ((0xffffffff - log_period) >= thistime) )
      next_log += log_period;
  }


  // Figure out whether either of the light sensors are above-threshold.

  opt_left = false;
  opt_right = false;

  if (LIGHT_CAL_SRC_RIGHT == opt_thresh_src)
    opt_thresh_L = opt_thresh_R;
  else if (LIGHT_CAL_SRC_LEFT == opt_thresh_src)
    opt_thresh_R = opt_thresh_L;

  if (opt_L > opt_thresh_L)
    opt_left = true;
  if (opt_R > opt_thresh_R)
    opt_right = true;


  // If events happened, report them.


  // Logging.

  if (need_report)
  {
    switch (verbosity)
    {
      case VERB_PACKED:
        // Packed hex.

        PrintConstString("T");
        PrintHex32(thistime);

        // Wrap the analog integer printing to use appropriate fixed widths,
        // depending on sample size setting.
        PrintConstString("J");
        ANALOGPRINT(joy_X);
        ANALOGPRINT(joy_Y);
        ANALOGPRINT(joy_C);

        PrintConstString("L");
        ANALOGPRINT(opt_L);
        ANALOGPRINT(opt_R);

        if (opt_left)
          PrintConstString("W");
        else
          PrintConstString("B");

        if (opt_right)
          PrintConstString("W");
        else
          PrintConstString("B");

        break;

      case VERB_TERSE:
        // Terse but human-readable.

        PrintConstString("T: ");
        PrintHex32(thistime);

        // Wrap the analog integer printing to use appropriate fixed widths,
        // depending on sample size setting.
        PrintConstString("  XYC: ");
        ANALOGPRINT(joy_X);
        PrintConstString(" ");
        ANALOGPRINT(joy_Y);
        PrintConstString(" ");
        ANALOGPRINT(joy_C);

        PrintConstString("  LR: ");
        ANALOGPRINT(opt_L);
        PrintConstString(" ");
        ANALOGPRINT(opt_R);

        if (opt_left)
          PrintConstString(" W");
        else
          PrintConstString(" B");

        if (opt_right)
          PrintConstString(" W");
        else
          PrintConstString(" B");

        break;

      default:
        // Verbose.

        PrintConstString("Time: ");
        PrintHex32(thistime);

        // Wrap the analog integer printing to use appropriate fixed widths,
        // depending on sample size setting.
        PrintConstString("  Joy (x/y/c): ");
        ANALOGPRINT(joy_X);
        PrintConstString(" ");
        ANALOGPRINT(joy_Y);
        PrintConstString(" ");
        ANALOGPRINT(joy_C);

        PrintConstString("  Opt (l/r): ");
        ANALOGPRINT(opt_L);
        PrintConstString(" ");
        ANALOGPRINT(opt_R);

        if (opt_left)
          PrintConstString(" WHT");
        else
          PrintConstString(" BLK");

        if (opt_right)
          PrintConstString(" WHT");
        else
          PrintConstString(" BLK");

        break;
    }

    PrintConstString("\r\n");
  }

  // Reward pulse.

  if (had_reward)
  {
    PrintConstString("Reward: ");
    PrintHex32(reward_time);
    PrintConstString(" ");
    PrintHex16(reward_length);
    PrintConstString("\r\n");
  }

  if (had_reward2)
  {
    PrintConstString("Reward 2: ");
    PrintHex32(reward2_time);
    PrintConstString(" ");
    PrintHex16(reward2_length);
    PrintConstString("\r\n");
  }

  // Timing pulse.

  if (had_timing)
  {
    PrintConstString("Synch: ");
    PrintHex32(timing_time);
    PrintConstString("\r\n");
  }

  if (had_timing2)
  {
    PrintConstString("Synch 2: ");
    PrintHex32(timing2_time);
    PrintConstString("\r\n");
  }

  // Event code handshaking.
  if (had_evcode)
  {
    PrintConstString("Code: ");
    PrintHex32(evcode_time_copy);
    PrintConstString(" ");
    PrintHex16(evcode_data_copy);
    PrintConstString("\r\n");
  }

  // Debugging.
  // FIXME - This can be a firehose!
  if (debug_light_samples && analog_new_round)
  {
    // Packed output: "*TTTTLLLLRRRR\r\n".
    // Timestamp is LS 16 bits of tick count.
    // FIXME - Removing "*". 10 chars vs 11 chars at 8 bits/sample makes
    // all the difference at 115.2 kbaud.
//    PrintConstString("*");
    PrintHex16((thistime & 0xffff));
    ANALOGPRINT(opt_L);
    ANALOGPRINT(opt_R);
    PrintConstString("\r\n");
  }
}



// Updates noise filter configuration.

void ConfigureNoiseFilters(uint8_t new_fuzz_bits, uint8_t new_var_bits,
  uint8_t new_spike_bits)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // For now, everything gets the same constants.
    nfilt_joy_x.SetFilterTimes(new_fuzz_bits, new_var_bits, new_spike_bits);
    nfilt_joy_y.SetFilterTimes(new_fuzz_bits, new_var_bits, new_spike_bits);
    nfilt_joy_c.SetFilterTimes(new_fuzz_bits, new_var_bits, new_spike_bits);
    nfilt_opt_left.SetFilterTimes(new_fuzz_bits, new_var_bits,
      new_spike_bits);
    nfilt_opt_right.SetFilterTimes(new_fuzz_bits, new_var_bits,
      new_spike_bits);
  }
}



// Sets swapped/not-swapped state for light sensors and joysticks.
// FIXME - This is a workaround for filter hardware issues.

void SetOptJoySwapState(bool is_swapped)
{
  if (is_swapped)
  {
    analog_input_lut = analog_input_lut_swapped;
    opt_l_pin_id = JOY_X_PIN;
    opt_r_pin_id = JOY_Y_PIN;
    light_joystick_swapped = true;
  }
  else
  {
    analog_input_lut = analog_input_lut_normal;
    opt_l_pin_id = OPT_L_PIN;
    opt_r_pin_id = OPT_R_PIN;
    light_joystick_swapped = false;
  }
}


//
// This is the end of the file.
