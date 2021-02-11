// Attention Circuits Control laboratory - I/O SynchBox project
// ADC routines - Mega 2560
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Typedefs

// ADC data width depends on configuration.
#if ANALOG_16BIT
typedef uint16_t analog_sample_t;
#else
typedef uint8_t analog_sample_t;
#endif


//
// Global variables

// ADC sample.
// This is volatile, because ISRs can update it.
extern volatile bool adc_data_valid;
extern volatile analog_sample_t adc_data;


//
// Classes

// NOTE - This might have to be changed to a struct for speed's sake.
class NoiseFilter
{
protected:
  // Data might be 16-bit, so use 32-bit for internal state.
  // NOTE - Variance still needs special handling to not overflow.
  uint32_t averagefuzz, averagevar, variance;
  analog_sample_t last_valid;
  uint8_t fuzz_lpf_bits, var_lpf_bits, spike_shift_bits;
  bool need_init;

public:
  NoiseFilter(void);
  // Default destructor is fine.

  // Reconfigure filter parameters.
  // NOTE - This must be called within an atomic block!
  void SetFilterTimes(uint8_t new_fuzz, uint8_t new_var, uint8_t new_spike);

  // Processes a sample, returning the filtered value.
  analog_sample_t FilterSample(analog_sample_t sample_value);
};


//
// Functions


// Initializes the analog to digital converter.
void InitADC();

// Performs ADC housekeeping.
// This should be called from within an ISR, or else wrapped in a lock.
// This will notice when an ADC conversion completes and copy the
// data to a safe location, resetting the ADC.
// Returns true if the ADC is available (idle) and false if it's busy.
bool PollADC_ISR();

// Sets up an ADC conversion using the specified input.
// This should be called from within an ISR, or else wrapped in a lock.
// NOTE - This should only be called when PollADC_ISR() indicates the ADC is free!
void StartAnalogRead_ISR(int pin_id);


//
// This is the end of the file.
