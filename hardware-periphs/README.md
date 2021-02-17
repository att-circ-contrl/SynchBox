# "I/O SynchBox" Hardware -- Peripheral Devices

## Overview

Various peripherals have been built for the "I/O SynchBox". Most of these
are used to map TTL outputs from the rectangular connector on the SyncBox
front panel to TTL inputs on other pieces of equipment. This allows "event
codes" to be sent from the SynchBox that are used to align data recorded by
the SynchBox's host computer with data recorded by these other devices.

Other peripherals exist and are noted below.


## Folders

* `adapter-brainamp` --
Adapter for a 3-row 26-pin D-sub connector used by the Brain Products
"BrainAmp" device.
* `adapter-intan` --
Adapter mapping 8-bit 10-position cables (from the "`ribbon-splitter`"
adapter) to pins that mate with the digital I/O terminal blocks on the
Intan "I/O Expander" used with Intan's "RHD" and "RHS" series recording and
stimulation controllers (respectively).
* `adapter-neuroscan` --
Adapter for a 25-pin D-sub connector used by the Compumedics "NeuroScan"
device.
* `adapter-tobii` --
Combination splitter/adapter mapping the least-significant 8 bits of the
SynchBox's TTL output to a 9-pin D-sub connector used by the Tobii "Pro
Spectrum" eye-tracker. All TTL ouputs are also passed through to a second
ribbon cable connector.
* `chuck-standalone` --
Hardware and firmware for a stand-alone device that communicates with a host
computer in a similar manner to the SynchBox, relaying data from a Nintendo
"Wii Nunchuck" joystick.
* `joystick-2016` --
Scematics for a ruggedized analog joystick used in 2016 epxierments.
* `light-sensor-clamp` --
3d printed clamp used for mounting light sensors on the corners of monitors.
Patches of the screen under the light sensor are strobed to encode data, and
the SynchBox reads the resulting analog signals to allow timing
reconstruction with sub-frame accuracy.
* `ribbon-splitter` --
Splitter converting the 16-bit 34-position rectangular connector TTL output
to two 8-bit 10-position rectangular connector outputs.


_This is the end of the file._
