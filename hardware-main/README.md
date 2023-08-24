# "I/O SynchBox" Hardware -- Main Unit

## Overview

The "I/O SynchBox" consists of a "shield" that sits on top of an "Arduino
Mega 2560" board, with ribbon-cable headers connecting the shield to
various faceplate boards. This decouples the shield design, faceplate board
design, and choice of enclosure (each may be varied without needing to
redesign the other, as long as headers are kept consistent).

**NOTE** -- Some or all of these boards will eventually be replaced with
general-purpose variants in their own project.


## Folders

* `mech-aux` --
Folder for mechanical models used by several components.
* `notes` --
Folder for auxiliary information for building the SynchBox.
* `v2-backpanel` --
Faceplate board with 8 TTL BNC connectors and a USB 2.0 B
power-and-communication port.
Fits in Hammond's aluminum box end-panels, should fit a 1u rack panel.
* `v2-frontpanel` --
Faceplate board with 5 analog input BNC ports, reset switch, status light,
and a rectangular connector with 16-bit digital output.
Fits in Hammond's aluminum box end-panels, should fit a 1u rack panel.
* `v2-shield` --
"Shield" board for use with an "Arduino Mega 2560" board. Maps analog
header pairs 4 and 5 to light sensor circuitry (pull-ups and pre-amplifier
circuits).


_This is the end of the file._
