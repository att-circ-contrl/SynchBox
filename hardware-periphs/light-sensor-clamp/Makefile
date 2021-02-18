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
DSTBASE=light-sensor

UPATH=../../../../utils/

clean:
	rm -f packages/*

release: clean
	cd meshes; zip ../packages/$(DSTBASE)-meshes.zip *stl
	cp notes/light-sensor-bom.txt packages/$(DSTBASE)-bom.txt
	cp notes/NOTES-assembly packages/$(DSTBASE)-assembly.txt
	cp renders/clamp-assembly.png packages/$(DSTBASE)-assembly.png

#
# This is the end of the file.
