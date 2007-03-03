#!/bin/sh
echo "#declare BallColor = $2" >clr.inc
#### jumping balls
povray -w300 -h300 -V1 -P +A +KI10 +KF11 +KFF13 -Iball.pov +Oa$1.tga
#### borning balls
povray -w300 -h300 -V1 -P +A +KI5 +KF6 +KFF6 +SF2 +EF5 -Iball.pov +Ob$1.tga
#### burning balls balls
povray -w300 -h300 -V1 -P +A +KI0 +KF1 +KFF11 +SF1 +EF5 -Iball.pov +Oe$1.tga

montage -geometry 300x300 -tile 100x1 -page 5700x300 a$1??.tga b$1?.tga e$1??.tga bl_$1.tga
mogrify  -crop 5700x300+0+0 bl_$1.tga


