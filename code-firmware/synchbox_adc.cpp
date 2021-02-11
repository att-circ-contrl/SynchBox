// Attention Circuits Control laboratory - I/O SynchBox project
// ADC routines - Mega 2650
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "synchbox_includes.h"



//
// Global variables

// ADC sample.
// This is volatile, because ISRs can update it.
volatile bool adc_data_valid;
volatile analog_sample_t adc_data;



//
// Constants

// Reference portion of ADMUX.
// We're using the internal 2.56V reference.
// Results are left-adjusted, because we only need the MS 8 bits.
// We need a capacitor on the AREF pin; the Mega 2560 board has one.
#define ADMUX_BASE 0xe0

// Analog output pin LUT for ADMUX.
// NOTE - Input pins 8..15 require setting MUX5, which is in ADCSRB.
// Make life easier and limit ourselves to 0..7, so that MUX5 is always 0.
#define ADMUX_PIN_COUNT 8
const uint8_t admux_lsb_lut[ADMUX_PIN_COUNT] =
{
  // Conveniently, code = input number for single-ended inputs 0..7.
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07
};

// We want the comparator off, MUX5 to 0, and trigger to free running (0).
// Trigger mode is actually ignored, as we're disabling auto-triggering.
#define ADCSRB_VALUE 0x00

// We want three states: ADC off, ADC ready, and ADC start conversion.
// Remember that we need to write 1 to ADIF to clear it, not 0.
// That said, we can ignore ADIF and just watch ADSC.
// We want a clock divisor of /128, for an ADC clock of 125 kHz (9.6 ksps).
#define ADCSRA_OFF 0x17
#define ADCSRA_READY (ADCSRA_OFF | 0x80)
#define ADCSRA_STARTMASK 0x40
#define ADCSRA_START (ADCSRA_READY | ADCSRA_STARTMASK)



//
// Functions


// Initializes the analog to digital converter.

void InitADC()
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    adc_data_valid = false;
    adc_data = 0;

    // FIXME - All of this is m2560-specific.

    // Disable digital inputs.
    DIDR0 = 0x00;
    DIDR2 = 0x00;
    // DIDR1 is for the comparator, not the ADC.

    // Make sure the ADC is off.
    ADCSRA = ADCSRA_OFF;

    // Set the remaining control registers to reasonable values.
    ADCSRB = ADCSRB_VALUE;
    ADMUX = ADMUX_BASE | admux_lsb_lut[0];

    // Turn the ADC on.
    // NOTE - This may take a little while to stabilize!
    // Don't trust the first few conversion values.
    // The first conversion will also take about twice as long.
    ADCSRA = ADCSRA_READY;
  }
}


// Performs ADC housekeeping.
// This should be called from within an ISR, or else wrapped in a lock.
// This will notice when an ADC conversion completes and copy the
// data to a safe location, resetting the ADC.
// Returns true if the ADC is available (idle) and false if it's busy.

bool PollADC_ISR()
{
  uint8_t adcsra_value;
  uint16_t adcval_high;
  uint16_t adcval_low;

  // No lock needed, as we're being called from an ISR.

  // NOTE - Only check this if we haven't _already_ checked it!
  if (!adc_data_valid)
  {
    // See if a conversion has completed.
    adcsra_value = ADCSRA;
    if (0 == (adcsra_value & ADCSRA_STARTMASK))
    {
#if ANALOG_16BIT
      // Record the full 16 bits.
      // This is still left-adjusted, so 0xffff is 2.56V.
      // Read LS, then MS; MS resets.
      adcval_low = ADCL;
      adcval_high = ADCH;
      adc_data = (adcval_high << 8) | (adcval_low);
#else
      // Record the MS byte of this sample.
      // It's left-adjusted, so this gives us the top 8 bits.
      adc_data = ADCH;
#endif

      // We now have valid data.
      adc_data_valid = true;
    }
  }

  return adc_data_valid;
}


// Sets up an ADC conversion using the specified input.
// This should be called from within an ISR, or else wrapped in a lock.
// NOTE - This should only be called when PollADC_ISR() indicates the ADC is free!

void StartAnalogRead_ISR(int pin_id)
{
  uint8_t admux_value;

  if ((0 <= pin_id) && (ADMUX_PIN_COUNT > pin_id))
  {
    // Select our input.
    admux_value = admux_lsb_lut[pin_id];
    admux_value |= ADMUX_BASE;
    ADMUX = admux_value;

    // Clear data flags and record.
    adc_data_valid = false;
    adc_data = 0;

    // Trigger the ADC.
    ADCSRA = ADCSRA_START;
  }
}



//
// Class Implementations


// Constructor.
// Initialize to stable defaults. Set default bit values here too.
// Flag as needing further initialization.

NoiseFilter::NoiseFilter(void)
{
  need_init = true;
  averagefuzz = 0;
  averagevar = 0;
  variance = 0;
  last_valid = 0xaa; //FIXME - Debug value.
  fuzz_lpf_bits = AN_FILTER_LPF_BITS_DEFAULT;
  var_lpf_bits = AN_FILTER_SPIKE_LPF_BITS_DEFAULT;
  spike_shift_bits = AN_FILTER_SPIKE_SIZE_BITS_DEFAULT;
}


// Reconfigure filter parameters.
// NOTE - This should be done within an atomic block!

void NoiseFilter::SetFilterTimes(uint8_t new_fuzz, uint8_t new_var,
  uint8_t new_spike)
{
  // Copy the new state information over and flag as needing reinitialization.
  fuzz_lpf_bits = new_fuzz;
  var_lpf_bits = new_var;
  spike_shift_bits = new_spike;
  need_init = true;
}


// Processes a sample, returning the filtered value.

analog_sample_t NoiseFilter::FilterSample(analog_sample_t sample_value)
{
  uint32_t this_sample, this_variance;
  uint32_t scratch, roundfuzz, roundvar;
  uint32_t fuzz_output, dcavg_output, var_output;

  if (need_init)
  {
    need_init = false;

    // Pretend the variance is zero, and that the sample is our average.

    last_valid = sample_value;

    this_sample = sample_value;

    averagefuzz = this_sample << fuzz_lpf_bits;
    averagevar = this_sample << var_lpf_bits;
    variance = 0;
  }
  else
  {
    // Promote the sample to 32 bits.
    this_sample = sample_value;

    // Precalculate rounding constants, to avoid repetition.
    roundfuzz = 0;
    roundvar = 0;
    if (0 < fuzz_lpf_bits)
      roundfuzz = (1 << (fuzz_lpf_bits - 1));
    if (0 < var_lpf_bits)
      roundvar = (1 << (var_lpf_bits - 1));


    // If we're doing spike suppression, update the variance using the _old_
    // DC average.
    if (0 == var_lpf_bits)
      this_variance = 0;
    else
    {
      dcavg_output = (averagevar + roundvar) >> var_lpf_bits;

      // FIXME - This would be easier with signed values.
      if (this_sample > dcavg_output)
        this_variance = this_sample - dcavg_output;
      else
        this_variance = dcavg_output - this_sample;
      this_variance *= this_variance;
      // Knock a few bits off of it to make sure filtering fits in uint32_t.
      this_variance >>= var_lpf_bits;
    }


    // Update the LPF states for variance and for fuzz suppression.
    // Don't do outlier rejection for this; update using all data.
    // If either filter is disabled, treat this sample as that average.

    if (0 == fuzz_lpf_bits)
      fuzz_output = this_sample;
    else
    {
      scratch = (averagefuzz + roundfuzz) >> fuzz_lpf_bits;
      averagefuzz -= scratch;
      averagefuzz += this_sample;

      fuzz_output = (averagefuzz + roundfuzz) >> fuzz_lpf_bits;
    }

    // DC average is calculated based on raw data, not de-fuzzed data.
    if (0 == var_lpf_bits)
      // This gets stomped when we reinitialize, so just set it to zero here.
      averagevar = 0;
    else
    {
      scratch = (averagevar + roundvar) >> var_lpf_bits;
      averagevar -= scratch;
      averagevar += this_sample;
    }


    // If this is not an outlier, update most-recent-valid-output.
    // Always update the running variance, even if this is an outlier.
    // Otherwise we can get locked into a situation where almost everything
    // is an outlier.
    if (0 == var_lpf_bits)
      last_valid = (analog_sample_t) fuzz_output;
    else
    {
      scratch = (variance + roundvar) >> var_lpf_bits;
      variance -= scratch;
      variance += this_variance;

      var_output = (variance + roundvar) >> var_lpf_bits;

      // Only update last_valid if we _don't_ have an outlier.
      // Otherwise keep the old value.
      if ((this_variance >> spike_shift_bits) <= var_output)
        last_valid = (analog_sample_t) fuzz_output;
    }
  }

  // Return the most recent valid value.
  return last_valid;
}


//
// This is the end of the file.
