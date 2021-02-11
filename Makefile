# Attention Circuits Control Laboratory - I/O SynchBox project
# Top-level Makefile.
# Written by Christopher Thomas.
# Copyright (c) 2021 by Vanderbilt University. This work is released under
# the Creative Commons Attribution-ShareAlike 4.0 International License.

default: helpscreen

helpscreen:
	@echo ""
	@echo "Targets:   manual  hex  burn"
	@echo ""

manual:
	rm -f manuals/*pdf
	make -C manuals-src clean all
	mv manuals-src/*pdf manuals

# This rebuilds the hex file, copies it, then cleans up again.
hex:
	make -C code-firmware -f Makefile.neuravr clean hex
	make -C code-firmware -f Makefile.neuravr clean

burn:
	make -C hexfiles burnard


# FIXME - Make "bundle PCB release" targets here too?


#
# This is the end of the file.
