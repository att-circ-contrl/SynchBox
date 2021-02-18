#!/bin/bash

if (( $# != 2 ))
then

echo ""
echo "Usage:  pcb2png <base gerber filename> <image filename>"
echo ""
echo "This uses \"gerbv\" to render a PNG image file containing a PCB layout."
echo ""

else

GBASE=$1
PNGNAME=$2

GVARGS="--export=png -o $PNGNAME --dpi=150 --background=#ffffff"

# Layers mentioned first are "higher" in the layer stack.

# FIXME - We really want to multiply layer bitmaps together.
# In theory we have the option of ANDing them.
# In practice, alpha is all we'll get.
# For blending, we blend with the background too, desaturating.

# FIXME - Not supporting 4-layer boards yet!

if [ -e $GBASE.outline.gbr ]
then
  # Overwrite.
  GVARGS="$GVARGS -f#808080ff $GBASE.outline.gbr"
fi

if [ -e $GBASE.topsilk.gbr ]
then
#  GVARGS="$GVARGS -f#e0e0a0ff $GBASE.topsilk.gbr"
  # Blend.
  GVARGS="$GVARGS -f#c0c00080 $GBASE.topsilk.gbr"
fi

if [ -e $GBASE.top.gbr ]
then
#  GVARGS="$GVARGS -f#ff00ffff $GBASE.top.gbr"
  # Blend.
  GVARGS="$GVARGS -f#f000f080 $GBASE.top.gbr"
fi

if [ -e $GBASE.bottomsilk.gbr ]
then
#  GVARGS="$GVARGS -f#c0c080ff $GBASE.bottomsilk.gbr"
  # Blend.
  GVARGS="$GVARGS -f#80800080 $GBASE.bottomsilk.gbr"
fi

if [ -e $GBASE.bottom.gbr ]
then
  # Overwrite. We're the bottom layer.
  GVARGS="$GVARGS -f#00f0f0ff $GBASE.bottom.gbr"
fi

#echo "Args: $GVARGS"
gerbv $GVARGS

fi
