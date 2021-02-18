# Attention Circuits Control Laboratory - I/O Synchbox
# Hardware component Makefile.
# Written by Christopher Thomas.
# Copyright (c) 2021 by Vanderbilt University. This work is released under
# the Creative Commons Attribution-ShareAlike 4.0 International License.

default: helpscreen

helpscreen:
	@echo ""
	@echo "Targets:  release"
	@echo ""


SRCBASE=
DSTBASE=nunchuck

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd mech/case-v1b; zip ../../packages/$(DSTBASE)-gcode.zip *txt
	cp hexfiles/neurochuck-arduinodev.hex packages/$(DSTBASE)-image.hex
	cp drawings/chuck-v1-mech.png packages/$(DSTBASE)-drawing.png

#
# This is the end of the file.
