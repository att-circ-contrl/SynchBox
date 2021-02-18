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


SRCBASE=pcb-evbig
DSTBASE=ribbon-splitter

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd boards/gerber; $(UPATH)/pcb2seeed.sh $(SRCBASE) $(DSTBASE); mv *zip ../../packages/$(DSTBASE)-pcb.zip
	cd boards/gerber; $(UPATH)/pcb2png.sh $(SRCBASE) $(DSTBASE)-layout.png; mv *png ../../packages
	cp notes/ribbon-splitter-bom.txt packages/$(DSTBASE)-bom.txt
	cp notes/pinout.txt packages/$(DSTBASE)-pinout.txt
	cd meshes; zip ../packages/$(DSTBASE)-meshes.zip *stl
	cp renders/event-cable-adapter-vert.png packages/$(DSTBASE)-render.png
	cp drawings/evbig-mech.png packages/$(DSTBASE)-drawing.png

#
# This is the end of the file.
