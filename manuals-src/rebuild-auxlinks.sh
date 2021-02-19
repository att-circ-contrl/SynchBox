#!/bin/bash

# This rebuilds symlinks to packages, photos, etc. used by the manual.


# Schematics.

rm -f schematics/*
cd schematics
for F in ../../packages/*/*schem*
do
  ln -s $F
done
cd ..


# Layouts.

rm -f layouts/*
cd layouts
for F in ../../packages/*/*layout*
do
  ln -s $F
done
cd ..


# Header pinouts.
# Some of these are singular ("-pinout.png") and some plural ("-pinouts.png").
# Make sure to only link images, not text files.

rm -f pinouts/*
cd pinouts
for F in ../../packages/*/*pinout*g
do
  ln -s $F
done
cd ..


#
# This is the end of the file.
