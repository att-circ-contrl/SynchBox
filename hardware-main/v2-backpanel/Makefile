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


SRCBASE=v2-bnc-gpio
DSTBASE=synchbox-backpanel

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd boards/gerber; $(UPATH)/pcb2seeed.sh $(SRCBASE) $(DSTBASE); mv *zip ../../packages/$(DSTBASE)-pcb.zip
	cd boards/gerber; $(UPATH)/pcb2png.sh $(SRCBASE) $(DSTBASE)-layout.png; mv *png ../../packages
	cp drawings/v2-backpanel.pdf packages/$(DSTBASE)-drawing.pdf
	cd mech/carve-king; zip ../../packages/$(DSTBASE)-gcode.zip *gcode
	cp meshes/v2-backpanel.stl packages/$(DSTBASE)-mesh.stl
	cp iges/v2-backpanel.iges packages/$(DSTBASE)-model.iges
	cp notes/v2-backpanel-bom.txt packages/$(DSTBASE)-bom.txt

#
# This is the end of the file.
