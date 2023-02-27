# Attention Circuits Control Laboratory - Makefile for burning hex files.
# Written by Christopher Thomas.

#
# Configuration.

MEGAFLAGS=-p m2560

# This looks for an Atmel AVR ISP Mk 2.
ISPFLAGS=-c avrispv2 -P usb

# This looks for a boot loader (pretending to be a stk500).
# FIXME - This blithely assumes we're on the first USB serial device and
# are using Linux Mint 18's naming conventions.
# FIXME - Some clones are /dev/ttyUSB0, not /dev/ttyACM0.
# NOTE - We need "-D" to cancel flash erase, as the boot loader refuses to
# erase itself.
ARDFLAGS=-c stk500 -P /dev/ttyACM0 -D
CLONEFLAGS=-c stk500 -P /dev/ttyUSB0 -D


#
# Hex files.

HEXSYNCH=synchbox.hex


#
# Targets.

default: helpscreen

helpscreen:
	@echo ""
	@echo "Targets:   burnisp  burnard  test"
	@echo ""


burnisp: $(HEXSYNCH)
	avrdude $(ISPFLAGS) $(MEGAFLAGS) -U flash:w:$(HEXSYNCH):i

burnard: $(HEXSYNCH)
	avrdude $(ARDFLAGS) $(MEGAFLAGS) -U flash:w:$(HEXSYNCH):i

burnclone: $(HEXSYNCH)
	avrdude $(CLONEFLAGS) $(MEGAFLAGS) -U flash:w:$(HEXSYNCH):i


# Using "cu -h" for manual echo.
# FIXME - Some clones are /dev/ttyUSB0, not /dev/ttyACM0.
# Some clones may also not support flow control.
test:
	cu -h -l /dev/ttyACM0 -s 115200

testclone:
	cu -h -f -l /dev/ttyUSB0 -s 115200


#
# This is the end of the file.
