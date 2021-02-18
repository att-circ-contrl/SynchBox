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


SRCBASE=adapter-neuroscan
DSTBASE=adapter-neuroscan

UPATH=../../../../utils/

clean:
	rm -f packages/*

# FIXME - No BOM. Layout has annotations for part numbers.
release: clean
	cd boards/gerber; $(UPATH)/pcb2seeed.sh $(SRCBASE) $(DSTBASE); mv *zip ../../packages/$(DSTBASE)-pcb.zip
	cd boards/gerber; $(UPATH)/pcb2png.sh $(SRCBASE) $(DSTBASE)-layout.png; mv *png ../../packages
	cp drawings/$(SRCBASE)-faceplate.eps packages/$(DSTBASE)-faceplate.eps
	cp drawings/$(SRCBASE)-shell.eps packages/$(DSTBASE)-shell.eps
	cd mech; zip ../packages/$(DSTBASE)-gcode.zip *txt

#
# This is the end of the file.
