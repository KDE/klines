#declare CELLSIZE = 32
#include "colors.inc"

camera
{
    orthographic
    location  <0,0,-100>
    look_at   <0,0,0>
    up (CELLSIZE-2)*y
    right (CELLSIZE)*x
}
// Global settings
global_settings
{
     ambient_light rgb<0.5,0.5,0.5>
}
light_source
{
    <-400, 600, -600>
    color White
    cylinder
    radius 100
    falloff 1000
    tightness 1
    point_at <0,0,0>
}
background
{
     color rgb<.5,.5,.5>
}

