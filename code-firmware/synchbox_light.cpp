// Attention Circuits Control laboratory - I/O SynchBox project
// Light sensor thresholding
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "synchbox_includes.h"



//
// Global variables

// These may be accessed by ISRs.

// Calibration source (slave to left, slave to right, or calibrate independently).
volatile light_cal_src_t light_cal_src;

// Thresholds for the light sensors.
// These are always 16-bit, even if analog_sample_t is 8-bit.
volatile uint16_t light_threshold_left, light_threshold_right;

// FIXME - Pins assigned to the light sensors, for LightCalUpdate_ISR's reference.
// These are advisory copies set by the command parser.
// Initialize these to sane values on instantiation.
volatile int opt_l_pin_id = OPT_L_PIN;
volatile int opt_r_pin_id = OPT_R_PIN;


// Calibration state (not calibrating, calibrating once, calibrating continuously).
volatile light_cal_state_t light_cal_state;

// Window size and the end time for the next window.
volatile uint32_t light_cal_windowsize;
volatile uint32_t light_cal_windowend;

// Sample information for this calibration window.
volatile uint32_t light_thresh_lefttotal, light_thresh_leftcount;
volatile uint32_t light_thresh_righttotal, light_thresh_rightcount;



//
// Functions


// Performs first-time initialization of light sensor calibration variables.

void LightCalFirstInit()
{
  // Set the default threshold to "never triggers".
  light_threshold_left = 0xffff;
  light_threshold_right = 0xffff;
  
  // Force state to something sane.
  light_cal_state = LIGHT_CAL_STATE_DONE;
  
  // Force source to something sane.
  light_cal_src = LIGHT_CAL_SRC_INDEP;
  
  // Everything else gets reinitialized when LightCalInit gets called.
}



// Reinitializes calibration. Called from within an ISR or atomic lock.
// One-shot and free-running require a window size; done ignores it.

void LightCalInit_ISR(light_cal_state_t new_state, uint16_t window_size)
{
  light_cal_state = new_state;
  
  // Squash window_size if we aren't calibrating.
  if (LIGHT_CAL_STATE_DONE == new_state)
    window_size = 0;
    
  light_cal_windowsize = window_size;
  light_cal_windowend = QueryTimer_ISR();
  light_cal_windowend += window_size;
  
  light_thresh_lefttotal = 0;
  light_thresh_leftcount = 0;
  light_thresh_righttotal = 0;
  light_thresh_rightcount = 0;
}



// Updates light calibration based on a new analog sample. Called from within an ISR.
// The sample may or may not be relevant to calibration; filtering occurs inside
// this function. The caller treats it as a black box.
void LightCalUpdate_ISR(int pin_id, uint16_t value, uint32_t timestamp)
{
  // Update statistics if a) we're doing calibration at all, and b) this is a
  // light sensor input.
  if (LIGHT_CAL_STATE_DONE != light_cal_state)
  {
    // Check to see if our window has expired.
    // If so, compute the new thresholds and update the calibration state.
    if (timestamp >= light_cal_windowend)
    {
      // Compute thresholds, if we have any samples.
      if (0 < light_thresh_leftcount)
      {
        light_thresh_lefttotal /= light_thresh_leftcount;
        light_threshold_left = (uint16_t) light_thresh_lefttotal;
      }
      if (0 < light_thresh_leftcount)
      {
        light_thresh_righttotal /= light_thresh_rightcount;
        light_threshold_right = (uint16_t) light_thresh_righttotal;
      }
      
      // Reinitialize statistics.
      light_thresh_lefttotal = 0;
      light_thresh_leftcount = 0;
      light_thresh_righttotal = 0;
      light_thresh_rightcount = 0;

      // Update the state.
      if (LIGHT_CAL_STATE_ONESHOT == light_cal_state)
      {
        // One-shot. We're done.
        light_cal_state = LIGHT_CAL_STATE_DONE;
        // Leave the window endpoint where it is.
      }
      else
      {
        // Free-running. Push the window back.
        light_cal_windowend += light_cal_windowsize;
      }
    }
    
    // If we're still in a valid sampling window, update based on this sample.
    if (LIGHT_CAL_STATE_DONE != light_cal_state)
    {
      if (opt_l_pin_id == pin_id)
      {
        light_thresh_lefttotal += (uint32_t) value;
        light_thresh_leftcount++;
      }
      else if (opt_r_pin_id == pin_id)
      {
        light_thresh_righttotal += (uint32_t) value;
        light_thresh_rightcount++;
      }
    }
  }
}



//
// This is the end of the file.
