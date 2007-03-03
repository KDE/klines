if (povray -w320 -h320 -V1 -P +A +K10 -Ifield.pov +Ofield.tga)
then  
 mogrify -raise 10x10 field.tga
fi
