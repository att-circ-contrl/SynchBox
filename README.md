# "I/O SynchBox" Data Logger and Timing Signal Generator

## Overview

The I/O SynchBox is a USB-connected device designed to synchronize and
collect data from several pieces of equipment:

* It can generate two independent timing signals (TTL pulse trains on BNC).
* It can assert individual TTL pulses on two independent BNC outputs
(typically used for control of external equipment).
* It can emit 16-bit data words on a rectangular (ribbon cable) connector.
This is typically used to send "event codes" to e-phys recording equipment
and/or to eye-tracker equipment.
* It can read analog inputs on three BNC ports ("X", "Y", and "Z"; typically
analog joystick connections).
* It can read from light sensors on two BNC ports ("L" and "R"; typically
connected to an experiment computer monitor for frame synchronization).

The SynchBox has an internal real-time-clock with 0.1 ms precision. Event
reports (with timestamps) are sent to the host computer over a USB serial
link. Analog inputs are polled at a user-configured rate, and reported
(with timestamps) at the polling interval.

Clock drift is nominally 100 parts per million or better (the precision of
the clock crystal on the "Arduino Mega 2560" microcontroller board).


## Quick-Start

** FIXME - Content goes here. **
* Have a top-level Makefile as with the NeuroCam project for common tasks.
* Have manuals and hex files built as with the NeuroCam.


## Folders

* `datasheets` -- Vendor-supplied datasheets. Under vendor copyright.
* `hexfiles` -- Firmware binaries for the SynchBox.
* `manuals` -- SynchBox documentation.
* `manuals-src` -- Source for rebuilding the SynchBox documentation.
* `notes` -- Assorted notes regarding SynchBox use and development.


_This is the end of the file._
