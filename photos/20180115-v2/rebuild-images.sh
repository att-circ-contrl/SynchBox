#!/bin/bash

if [ ! -e orig/marker ]
then
  echo "Can't find \"orig/\"."
else

  rm -f ard-v2-*jpg

  convert -crop 1280x1024+440+400 orig/Pic_0115_001.jpg ard-v2-front.jpg
  convert -crop 1280x1024+380+330 orig/Pic_0115_002.jpg ard-v2-back.jpg
  convert -crop 1280x1024+380+300 orig/Pic_0115_004.jpg ard-v2-top.jpg

fi

# This is the end of the file.
