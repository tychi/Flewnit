#!/bin/bash
echo "This will resize all jpg images in the current directory to 640x480 (or 480x640, as appropriate), an optional paramter will be prefixed to result files."
for i in *.png ;
do 
SIZEX=$(identify $i | egrep -o '[0-9]+x' | egrep -o '[0-9]+')
SIZEY=$(identify $i | egrep -o 'x[0-9]+' | egrep -o '[0-9]+')
if(test $SIZEX -ge $SIZEY;)then
  convert -size 640x480 $i -resize 640x480 $1_$i;
else
  convert -size 480x640 $i -resize 480x640 $1_$i;
fi
echo "resized image $1_$i created." ; 
done 

