#! /bin/sh
#### clean

clean.sh

#### render

field.sh
fire.sh
balls.sh Red "rgb<1,0,0>"
balls.sh Green "rgb<0,1,0>"
balls.sh Blue "rgb<0,0,1>"
balls.sh Yellow "rgb<1,1,0>"
balls.sh Violet "rgb<1,0,1>"
balls.sh Cyan "rgb<0,1,1>"
balls.sh Brown "rgb<1/2,1/3,0>"

#### montage balls ( 20x7  cells each 300x300)

montage -geometry 5700x300 -tile 1x10 bl_*.tga balls.tga
mogrify -crop 5700x2100+0+0 balls.tga

#### convert to jpeg

convert -quality 95 balls.tga balls.jpg
convert -quality 95 fire.tga fire.jpg
convert -quality 95 field.tga field.jpg


#### copy to data

cp *jpg ../

#### clean

#clean.sh
