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


SRCBASE=adapter-brainamp
DSTBASE=adapter-brainamp

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd boards/gerber; $(UPATH)/pcb2seeed.sh $(SRCBASE) $(DSTBASE); mv *zip ../../packages/$(DSTBASE)-pcb.zip
	cd boards/gerber; $(UPATH)/pcb2png.sh $(SRCBASE) $(DSTBASE)-layout.png; mv *png ../../packages
	cp drawings/$(SRCBASE)-drawing.png packages/$(DSTBASE)-drawing.png
	cp drawings/$(SRCBASE)-laser.eps packages/$(DSTBASE)-lasercut.eps
	cp drawings/$(SRCBASE)-label.pdf packages/$(DSTBASE)-label.pdf

#
# This is the end of the file.
