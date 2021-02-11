# Attention Circuits Control Laboratory - I/O SynchBox project
# Change log, bug list, and feature request list.
Written by Christopher Thomas.


## Bugs and feature requests:

* Debug monitor flagging analog changes doesn't work. Not sure why.

* Debug monitor of analog has unused "dead time" code.

* Need to add better GPIO support in general.

*  Allow joystick filtering to be configured separately from light sensors.

* Have Makefile targets for 115kbaud and 500kbaud hex files.

* Allow commands to be queued for release when light sensor state changes.

* Ben had measured long-term timestamp drift between Unity and Arduino of
about 600ppm. This seemed to be due to stepwise glitches. See 18-19 sept
2018 email chain.


## Abbreviated changelog (most recent changes first):

* 11 Mar 2020 --
Fixed subtle timing and atomic-lock issues with timing pulses, reward
pulses, and event codes.

* 28 Jan 2020 --
Added second reward and second timing channel. Made "strobe" optional for
event codes (full 16 bits without).

* 03 Oct 2018 --
Fixed potential locking bug with `StartAnalogRead_ISR()` with nested
interrupts.

* 03 Oct 2018 --
Rewrote digital filtering code to carry residue forward, fixing the
terrible slope quantization problem. Made filter bits per-instance.


_This is the end of the file._
