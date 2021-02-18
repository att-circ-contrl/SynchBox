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


SRCBASE=v2-shield
DSTBASE=synchbox-shield

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd boards/gerber; $(UPATH)/pcb2seeed.sh $(SRCBASE) $(DSTBASE); mv *zip ../../packages
	cd boards/gerber; $(UPATH)/pcb2png.sh $(SRCBASE) $(DSTBASE)-layout.png; mv *png ../../packages
	cp notes/v2-headers/v2-headers.png packages/$(DSTBASE)-headers.png
	cp schematics/neurarduino-schem.png packages/$(DSTBASE)-schem.png
	cp schematics/preamp-schem.png packages/$(DSTBASE)-schem-preamp.png

#
# This is the end of the file.
