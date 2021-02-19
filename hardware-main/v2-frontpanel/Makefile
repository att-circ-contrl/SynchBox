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


SRCBASE=v2-ain-gprect-stat
DSTBASE=synchbox-frontpanel

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd boards/gerber; $(UPATH)/pcb2seeed.sh $(SRCBASE) $(DSTBASE); mv *zip ../../packages/$(DSTBASE)-pcb.zip
	cd boards/gerber; $(UPATH)/pcb2png.sh $(SRCBASE) $(DSTBASE)-layout.png; mv *png ../../packages
	cp drawings/v2a-ain-gprect-stat.eps packages/$(DSTBASE)-drawing.eps
	cd mech/carve-king; zip ../../packages/$(DSTBASE)-gcode.zip *gcode
	cp notes/v2-frontpanel-bom.txt packages/$(DSTBASE)-bom.txt
	cp diagrams/code-port.png packages/$(DSTBASE)-pinout.png

#
# This is the end of the file.
