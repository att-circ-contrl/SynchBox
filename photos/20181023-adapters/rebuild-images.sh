#!/bin/bash

if [ ! -e orig/marker ]
then
  echo "Can't find \"orig/\"."
else

  rm -f adapter*jpg

  convert -rotate 270 -gamma 1.5 -crop 2400x2400+0+24 -resize 1200x1200 \
    orig/Neurarduino-BrainAmp_adapter_interior.jpg adapter-brainamp-int.jpg
  convert -gamma 1.5 -crop 2400x2400+0+768 -resize 1200x1200 \
    orig/neurarduino-brainamp-again.jpg adapter-brainamp-ext.jpg
  convert -crop 900x1200+0+300 \
    orig/Neurarduino-Neuroscan_adapter_external.jpg adapter-neuroscan-ext.jpg
  convert -gamma 1.5 -crop 900x1200+0+400 \
    orig/Neurarduino-Neuroscan_adapter_interior.jpg adapter-neuroscan-int.jpg
  convert -crop 3000x2400+82+124 -resize 1000x800 \
    orig/Neurarduino-Spectrum_adapter_external.jpg adapter-tobiispect-ext.jpg
  convert -rotate 270 -crop 3200x2400+32+48 -resize 1600x1200 \
    orig/Neurarduino-Spectrum_adapter_internal.jpg adapter-tobiispect-int.jpg

fi

# This is the end of the file.
