# Attention Circuits Control Laboratory - I/O SynchBox project
# Top-level Makefile.
# Written by Christopher Thomas.
# Copyright (c) 2021 by Vanderbilt University. This work is released under
# the Creative Commons Attribution-ShareAlike 4.0 International License.

default: helpscreen

helpscreen:
	@echo ""
	@echo "Targets:   manual  hex  burn  hwpkgs"
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

# This recurses to rebuild hardware release packages for each sub-project.
hwpkgs:
	make -C hardware-main release
	make -C hardware-periphs release

# Undocumented target: Remove the hardware release packages.
hwclean:
	make -C hardware-main clean
	make -C hardware-periphs clean

# Undocumented target: Rebuild photos.
# This requires my "photos-orig" source tree, which isn't in the repository.
# The rebuild scripts fail non-destructively if that's missing, but it'll
# still confuse the user.
pics:
	make -C photos rebuild

# Undocumented target: Remove photos.
picclean:
	make -C photos clean


#
# This is the end of the file.
