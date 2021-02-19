#!/bin/bash

if [ ! -e orig/marker ]
then
  echo "Can't find \"orig/\"."
else

  rm -f ard-panels-*jpg

  convert -crop 1800x750+160+570 orig/Pic_1017_224.jpg ard-panels-front-angle.jpg
  convert -crop 1600x500+250+600 orig/Pic_1017_226.jpg ard-panels-front-flat.jpg
  convert -crop 1700x800+150+350 orig/Pic_1017_229.jpg ard-panels-back-angle.jpg
  convert -crop 1300x400+450+700 orig/Pic_1017_231.jpg ard-panels-back-flat.jpg

fi

# This is the end of the file.
