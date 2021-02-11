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

The I/O SynchBox project is copyright (c) 2021 by Vanderbilt University,
and is released under the Creative Commons Attribution-ShareAlike 4.0
International License.


## Quick-Start

The top-level Makefile automates common operations (under Ubuntu-derived
Linux, at least):

* Type "`make`" without arguments for the Makefile target list.

* To rebuild the manuals (which shuld already be in the "`manuals`" folder),
type "`make manual`".

* To flash an I/O SynchBox using the firmware hex file, plug the device
into your computer and type "`make burn`".

* To rebuild the firmware for the I/O SynchBox, make sure the "NeurAVR"
project is installed and "`neuravr`" is symlinked at an appropriate
location (in the "`code-firmware`" folder or in the folder the "`SynchBox`"
project is installed in), and type "`make hex`".


## Remarks

This project is at a milestone state, and is in service, but future changes
are planned that will substantially affect the project's structure.

The present version will probably be kept as a branch, but the active version
will make major changes:

* For the time being, the firmware includes a shim that allows compiling
with the Arduino IDE instead of the NeurAVR libraries. This is not
guaranteed to be supported in the future (and might already be broken; I
haven't tested it recently).

* While the firmware does use the NeurAVR libraries, it does not presently
use the NeurAVR application skeleton. Version 2 will use that, along with
the ADC libraries and other code presently duplicated in the firmware
source. The end result will be cleaner and more compact but won't build with
the Arduino IDE (the application skeleton source would have to be transcluded
at compile-time, which may eventually be supported but not soon).

* Certain firmware features may be dropped or modified. In particular, event
code strobe  bits aren't actually used by any of our applications, and
timing pulse, reward pulse, and event code generation may be replaced with
more general GPIO signal/pattern generation code. The legacy command set will
probably be kept, for use-case compatibility, but the implementation will
change (and may be moved to a "GPIO device" application library).

* TTL input for timing synchronization really should be implemented (on BNC
lines and on the parallel header). The underlying GPIO implementation will
certainly support that when implemented.

* Right now, all of the printed circuit boards are project-specific. The
boards are very similar to those used for other ACC Lab projects, and will
eventually be merged into a single set of standard boards with as few
project-specific components as possible. When that happens, all PCB layouts
and front-panel mechanical plans that are not SynchBox-specific will be
moved to that project.


## Folders

* `code-firmware` -- Firmware source code for the SynchBox.
* `datasheets` -- Vendor-supplied datasheets. Under vendor copyright.
* `hexfiles` -- Firmware binaries for the SynchBox.
* `manuals` -- SynchBox documentation.
* `manuals-src` -- Source for rebuilding the SynchBox documentation.
* `notes` -- Assorted notes regarding SynchBox use and development.


_This is the end of the file._
