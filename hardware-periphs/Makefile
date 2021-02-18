# Attention Circuits Control Laboratory - I/O Synchbox
# Hardware component Makefile.
# Written by Christopher Thomas.
# Copyright (c) 2021 by Vanderbilt University. This work is released under
# the Creative Commons Attribution-ShareAlike 4.0 International License.

default: helpscreen

helpscreen:
	@echo ""
	@echo "Targets:  clean release"
	@echo ""

clean:
	make -C adapter-brainamp clean
	make -C adapter-intan clean
	make -C adapter-neuroscan clean
	make -C adapter-tobii clean
	make -C chuck-standalone clean
	make -C joystick-2016 clean
	make -C light-sensor-clamp clean
	make -C ribbon-splitter clean

release:
	make -C adapter-brainamp release
	make -C adapter-intan release
	make -C adapter-neuroscan release
	make -C adapter-tobii release
	make -C chuck-standalone release
	make -C joystick-2016 release
	make -C light-sensor-clamp release
	make -C ribbon-splitter release

#
# This is the end of the file.
