#! /bin/sh
echo "#declare BallColor = White" >clr.inc
if (povray -w30 -h30 -V1 -P +A +KI0 +KF1 +KFF11 +SF6 +EF10 -Iball.pov \
+Ofire.tga)  
then   # mogrify -raise 1x1 e$1??.tga
 montage -geometry 30x30 -tile 100x1 -page 150x30 fire*.tga fire.tga
 mogrify  -crop 150x30+0+0 fire.tga
# animate -delay 7 $1??.tga
fi
