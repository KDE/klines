/***************************************************************************
    ball.pov  -  script for rendering Kolor Lines graphics
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#version 3.0

// Objectname = Main View
// Objecttype = view

// This camera is build for ratio 451:277
#include "colors.inc"
#include "textures.inc"
#declare BallColor = Green
#include "clr.inc"

#declare CELLSIZE = 32
#declare PIXTIME = 10
#declare BALLDOWN = 3
#declare BALLSPACE = 4

#warning concat("XXXX clock:",str(clock,5,2),"\n")

#switch (clock)
// --------------------burning ball-------------------
 #range (0,1.001)

  #declare explosion = 1
  #declare jump = 0
  #declare n = 0

  #declare t0 = 0
  #declare t1 = 0.2
  #declare t2 = 0.5
  #declare t3 = 0.8
  #declare t4 = 1
  #declare tt = clock
  #if ( tt = t0 )
   //turbulence
   #declare tur = 0
   #declare trb = 0
   #declare trf = 0
   #declare pf  = 0
  #end
  #if ( tt > t0 & tt <= t1 )
   #declare tl = (tt - t0)/(t1-t0)
   #declare tur = 3+(0.2-3)*tl
   #declare trb = 0+(0-0)*tl
   #declare trf = 1+(3-1)*tl
   #declare pf  = 0+(0.3-0)*t1
  #end
  #if ( tt > t1 & tt <= t2 )
   #declare tl = (tt - t1)/(t2-t1)
   #declare tur = 0.2+(0.5-0.1)*tl
   #declare trb = 0+(1-0)*tl
   #declare trf = 3+(1-3)*tl
   #declare pf  = 0.3+(0.2-0.3)*t1
  #end
  #if ( tt > t2 & tt <= t3 )
   #declare tl = (tt - t2)/(t3-t2)
   #declare tur = 0.5+(2-0.5)*tl
   #declare trb = 1+(1-1)*tl
   #declare trf = 1+(1-1)*tl
   #declare pf  = 0.2-(0.0-0.2)*t1
  #end
  #if ( tt > t3 )
   #declare tl = (tt - t3)/(t4-t3)
   #declare tur = 2+(5-2)*tl
   #declare trb = 1+(1-1)*tl
   #declare trf = 1+(0-1)*tl
   #declare pf  = 0.0-(0.0-0.0)*t1
  #end
  // ball position / size
  #declare by = 0
  #declare bh = CELLSIZE-2-BALLSPACE
  #declare bw = CELLSIZE-2-BALLSPACE
 #break
// ----------------borning ball----------------------
 #range (5,6.001)

  #declare explosion = 0
  #declare jump = 0

  // fire turbulence
  #declare tur = 0
  // trancperancy ball
  #declare trb = 0
  // trancperancy fire
  #declare trf = 0
  // lambda
  #declare pf  = 0
  // relative clock
  #declare tt = clock - 5
  // y position
  #declare by = 0
  #declare r=sqrt(tt)
  #declare bh = (CELLSIZE-2-BALLSPACE)*r
  #declare bw = (CELLSIZE-2-BALLSPACE)*r
  // normal
  #declare n = (1-tt)*r*2

 #break

 // -------------jumping ball----------------------
 #range (10,11.001)
  #declare explosion = 0
  #declare jump = 1
  #declare tur = 0
  #declare trb = 0
  #declare trf = 0
  #declare pf  = 0
  #declare n = 0
  #declare tt = clock - 10
  // ball free fall: y = -(g * t^2)/2
  // ball jums on X*sin( sqrt(k)*(t0 - t1 + t ) )
  // t whole motion time
  // g - gravity acceleration
  // A - frei movement amplitude
  // BB way with deflation
  #declare A = BALLDOWN
  #declare B = BALLSPACE
  #declare ta = PIXTIME
  #declare k = (pi + 2*sqrt( B*(A+B))/A - acos( A/(A+2*B))   )/ta
  #declare k = k*k
  #warning concat("XXXX k:",str(k,5,2),"\n")
  #declare g = k*A*A/(2*(A+B))
  #warning concat("XXXX g:",str(g,5,2),"\n")
  #declare t1 = sqrt(2*B/g)
  #warning concat("XXXX t1:",str(t1,5,2),"\n")
  #declare X = A*(A+2*B)/(2*(A+B))
  #warning concat("XXXX X:",str(X,5,2),"\n")

  #declare tc = PIXTIME - tt*PIXTIME

  #warning concat("XXXX tc:",str(tc,5,2),"\n")
  // ball position
  #if (tc < t1)
   #declare by = B/2 - g*tc*tc/2
  #else
   #declare by = -B/2 - g/k - X*cos( sqrt(k)*(ta-tc))
  #end
  #if ( by < -2 )
   #declare bh=CELLSIZE-2+2*by
   #declare bw = CELLSIZE-2-BALLSPACE+(-by-2)
  #else
   #declare bh = CELLSIZE-2-BALLSPACE
   #declare bw = CELLSIZE-2-BALLSPACE
  #end

 #break

 #else
  #warning " ERROR, clock out of range !!! \n"

#end
 

#warning concat("XXXX clock:",str(clock,5,2),"\n")

#warning concat("XXXX tt:",str(tt,5,2),"\n")





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
    radius 200
    falloff 1000
    tightness 1
    point_at <0,0,0>
}
// Background
background
{
     color rgb<.5,.5,.5>
}

// fire
#if (explosion = 1) 
sphere { 0, 1
    pigment { color rgbt<0,0,0,1>
    }
    halo {
      emitting
      spherical_mapping
      poly
      max_value 40
      exponent 0.1
      linear
      turbulence tur
//      phase pf
      lambda 2-pf
      frequency 1
      octaves 6
      color_map {
        [ 0 color rgbt <1, 0, 0, 1> ]
        [ 0.2 color rgbt <1, 0, 0, 1-1*trf> ]
        [ 0.5 color rgbt <1, 0, 0, 1-3*trf> ]
        [ 0.9 color rgbt <1, 1, 0, 1-4*trf> ]
        [ 1 color rgbt <1, 1, 0.5, 1-6*trf> ]
      }
      samples 30
    }
    hollow
    scale<15,15,15>
    translate <-0.5,-0.5,0>
  }
#end
// Ball
#if ( trb < 1 & bh > 0 )
sphere
{
    <0,0,0>,1
    pigment { 

    wood 
//    agate_turb 0.5
    turbulence .1
    frequency 2 
//    lambda 
    rotate <30,10,15>
    color_map {
      [0.5 BallColor transmit trb]
      [0.3 BallColor*0.95 transmit trb]
      [0.2 BallColor*0.9  transmit trb]
    }
    }
    no_shadow
    #if ( n > 0 )
    normal
    {
      //wood 0.5
      //frequency 2
      //turbulence .1
      //rotate <30,10,15>
      bumps n
      scale 0.3
    }
    #end
    finish {
    ambient 0.3
    diffuse 0.8
    specular 1
    roughness 0.001
    reflection .1
    phong 0.9 * ( 1-trb)
    phong_size 60

    }
    #if (trb > 0)
    hollow
    #end
    scale <bw/2,bh/2,bw/2>
    translate <-0.5,by - 0.5,0>

#warning concat("XXXX by:",str(by,5,2),"\n")
#warning concat("XXXX bh:",str(bh,5,2),"\n")
#warning concat("XXXX bw:",str(bw,5,2),"\n")
}
#end
