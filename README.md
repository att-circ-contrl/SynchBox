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

* To build release packages for all of the I/O SynchBox's hardware
components and peripherals, type "`make hwpkgs`". Board fabrication
files, meshes, and various documentation files are left in subfolders in
the "`packages`" directory.


## Remarks

This project is at a milestone state, and is in service, but future changes
are planned (eventually) that will substantially affect the project's
structure.

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
project-specific boards as possible. When that happens, all PCB layouts
and front-panel mechanical plans that are not SynchBox-specific will be
moved to that project.


## Folders

* `code-firmware` -- Firmware source code for the SynchBox.
* `datasheets` -- Vendor-supplied datasheets. Under vendor copyright.
* `hardware-main` -- Boards, CAM files, and drawings for the I/O SynchBox
internal components.
* `hardware-periphs` -- Boards, CAM files, meshes, and drawings for external
peripherals and accessories.
* `hexfiles` -- Firmware binaries for the SynchBox.
* `manuals` -- SynchBox documentation.
* `manuals-src` -- Source for rebuilding the SynchBox documentation.
* `notes` -- Assorted notes regarding SynchBox use and development.
* `packages` -- Automatically generated archives packaging relevant files
for making hardware components and peripherals. This typically includes a
`.zip` archive of the gerber files using the naming conventions of the
"Seeed Studio" PCB fabrication service, a `.png` rendering of the board
generated from the gerber files, a `.zip` archive containing g-code
toolpaths for various hardware parts, `.stl` files containing meshes for
various hardware parts, and other documentation as appropriate.
* `utils` -- Various helper scripts.
* `vendor-docs` -- Vendor-provided documentation (other than datasheets).
Under vendor copyright.


Within each hardware component's folder the following subdirectories may be
present:

* `boards` -- Printed circuit board design files. Anything with the `.pcb`
extension was made with the "`pcb`" program (now part of `gEDA`).
* `boards/gerber` -- Files for printed circuit board fabrication as generated
by `pcb`.
* `boards/plots` -- Manually assembled renderings of the circuit board layout.
* `cad` -- CAD files (mechanical models) for cases and faceplates. Files with
the `.fcstd` extension were made with the "`FreeCAD`" program. Files with
`.iges` and `.step` extensions are models exported to industry-standard
formats that can be imported into different CAD programs.
* `drawings` -- Mechanical drawings of boards, cases, and faceplates.
* `mech` -- CAM files (toolpaths) for cases and faceplates. Files with the
`.crv` extension were made with Vectric's `VCut` program. Files with the
`.crv3d` extension were made with Vectric's `Aspire` program but can probably
be imported into `VCut`. Files with the `.txt` extension are g-code; the tool
for each g-code file is usually indicated in the filename.
* `meshes` -- STL meshes for 3d printed parts.
* `notes` -- Assorted design documentation not covered above.
* `packages` -- A symbolic link to the project's sub-folder in the
top-level "`packages`" directory.
* `renders` -- Renders of mechanical models of cases, faceplates, and
assemblies.
* `schematics` -- Manually generated schematics for printed circuit boards.
Anything with the `.fig` extension was made with the "`xfig`" program.


Regarding the software tools used:

* I have a love/hate relationship with the `pcb` program. It works _just_
well enough that it isn't worth my time to learn a new program, but the
parts of the interface that I use have gone steadily downhill since 2003.
It also won't check a layout against a netlist, which is vital for anything
more complicated than the boards in this project. If you need to modify
existing board layouts, I suggest importing the gerber files into your
preferred PCB CAD program rather than trying to use the design files as-is.

* I have a love/hate relationship with the `FreeCAD` program. It works
_just_ well enough that it isn't worth my time to learn a new program, but
it has very strange ideas about how many operations should be done and it
lacks several features that would be very helpful for building models. If
you need to modify existing mechanical models, I suggest importing `.step`
or `.iges` models into your preferred CAD program.

* I've had good experiences with Vectric's CAM software, and the price for
`VCut` is reasonable for hobbyists. This generates 2D toolpaths at multiple
depths, so it's fine for cases and faceplates for electronics. That said,
it should be possible to convert EPS to DXF and import DXF into any CAM
program, or to reimplement the case designs very quickly using the mechanical
drawings, so any CAM program should be sufficient if you need to generate
new toolpaths.


_This is the end of the file._
