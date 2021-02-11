// Attention Circuits Control laboratory - I/O SynchBox project
// Light sensor thresholding
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Enums

enum light_cal_src_t
{
  LIGHT_CAL_SRC_RIGHT,
  LIGHT_CAL_SRC_LEFT,
  LIGHT_CAL_SRC_INDEP
};

enum light_cal_state_t
{
  LIGHT_CAL_STATE_DONE,
  LIGHT_CAL_STATE_ONESHOT,
  LIGHT_CAL_STATE_FREERUN
};


//
// Global variables

// These may be accessed by ISRs.

// Calibration source (slave to left, slave to right, or calibrate independently).
extern volatile light_cal_src_t light_cal_src;

// Thresholds for the light sensors.
// These are always 16-bit, even if analog_sample_t is 8-bit.
extern volatile uint16_t light_threshold_left, light_threshold_right;

// FIXME - Pins assigned to the light sensors, for LightCalUpdate_ISR's reference.
// These are advisory copies set by the command parser.
// This is a workaround for filter hardware issues.
extern volatile int opt_l_pin_id, opt_r_pin_id;


//
// Functions

// Performs first-time initialization of light sensor calibration variables.
void LightCalFirstInit();

// Reinitializes calibration. Called from within an ISR or atomic lock.
// One-shot and free-running require a window size; done ignores it.
void LightCalInit_ISR(light_cal_state_t new_state, uint16_t window_size);

// Updates light calibration based on a new analog sample. Called from within an ISR.
// The sample may or may not be relevant to calibration; filtering occurs inside
// this function. The caller treats it as a black box.
void LightCalUpdate_ISR(int pin_id, uint16_t value, uint32_t timestamp);


//
// This is the end of the file.
