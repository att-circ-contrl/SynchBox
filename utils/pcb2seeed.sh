#!/bin/bash

if (( $# != 2 ))
then

echo ""
echo "Usage:  pcb2seeed <base gerber filename> <project label>"
echo ""
echo "This creates a label.zip archive containing renamed gerber/drill files."
echo ""

else

GBASE=$1
PNAME=$2
ZIPNAME=$PNAME.zip

TMPDIR=/tmp/seeedtemp
THISDIR=`pwd`

rm -rf $TMPDIR
mkdir $TMPDIR

if [ -e $GBASE.bottom.gbr ]
then
  cp $GBASE.bottom.gbr $TMPDIR/$PNAME.GBL
fi

if [ -e $GBASE.bottommask.gbr ]
then
  cp $GBASE.bottommask.gbr $TMPDIR/$PNAME.GBS
fi

if [ -e $GBASE.bottomsilk.gbr ]
then
  cp $GBASE.bottomsilk.gbr $TMPDIR/$PNAME.GBO
fi

if [ -e $GBASE.top.gbr ]
then
  cp $GBASE.top.gbr $TMPDIR/$PNAME.GTL
fi

if [ -e $GBASE.topmask.gbr ]
then
  cp $GBASE.topmask.gbr $TMPDIR/$PNAME.GTS
fi

if [ -e $GBASE.topsilk.gbr ]
then
  cp $GBASE.topsilk.gbr $TMPDIR/$PNAME.GTO
fi

if [ -e $GBASE.outline.gbr ]
then
  cp $GBASE.outline.gbr $TMPDIR/$PNAME.GML
fi

# Handle both plated and unplated cases.

if [ -e $GBASE.plated-drill.cnc ]
then
  cp $GBASE.plated-drill.cnc $TMPDIR/$PNAME.TXT
fi

if [ -e $GBASE.unplated-drill.cnc ]
then
  cp $GBASE.unplated-drill.cnc $TMPDIR/$PNAME.TXT
fi

# Seeed Studio wants a TXT rather than CSV XY file, but this will have to do.

if [ -e $GBASE.xy ]
then
  cp $GBASE.xy $TMPDIR/$PNAME.POS
fi

cd $TMPDIR

zip $ZIPNAME $PNAME.G* $PNAME.T* $PNAME.P*

cd $THISDIR

rm -f $ZIPNAME
mv $TMPDIR/$ZIPNAME .

rm -rf $TMPDIR

fi
