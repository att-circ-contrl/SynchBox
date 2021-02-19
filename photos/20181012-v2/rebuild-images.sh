#!/bin/bash

if [ ! -e orig/marker ]
then
  echo "Can't find \"orig/\"."
else

  rm -f ard-v2-*jpg

  convert -crop 1900x900+140+500 orig/Pic_1012_207.jpg ard-v2-darkbg.jpg
  convert -crop 1900x900+140+430 orig/Pic_1012_211.jpg ard-v2-closed.jpg
  convert -crop 1800x1150+200+350 orig/Pic_1012_216.jpg ard-v2-open.jpg
  convert -crop 900x800+100+500 orig/Pic_1012_211.jpg ard-v2-clamps.jpg

fi

# This is the end of the file.
