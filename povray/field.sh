#!/bin/sh
if (povray -w32 -h32 -V1 -P +A +K10 -Ifield.pov +Ofield.tga)
then  
 mogrify -raise 1x1 field.tga
fi
