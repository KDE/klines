/***************************************************************************
                          ballpainter.cpp  -  description
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
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
#include <kapplication.h>
#include "ballpainter.moc"
//#include "shotcounter.h"
#include <qpainter.h>
#include "linesboard.h"
//#include <qcolor.h>
#include <qjpegio.h>
#include <kstandarddirs.h>


#define PIXSIZE (CELLSIZE - 2)

int colorLinesArr[NCOLORS] =
    {0x0000ff, 0x00ff00, 0xff0000, 0x00ffff, 0xff00ff, 0xffff00, 0x005080};
    // 0x00bbggrr
    // red   ,  green  ,  blue   , yellow  , violet  ,   cyan  , brown



BallPainter::BallPainter()
    : QObject()
{
    createPixmap();
}

BallPainter::~BallPainter()
{
}
void BallPainter::createPixmap()
{
  backgroundPix = new QPixmap(
		locate( "appdata", "field.jpg" ));
  QPixmap *balls = new QPixmap(
		locate( "appdata", "balls.jpg" ));
  QPixmap *fire = new QPixmap(
		locate( "appdata", "fire.jpg" ));
  if (balls->isNull() ||backgroundPix->isNull() || fire->isNull() )
     return; // Error

  for(int c=0; c<NCOLORS; c++)
  {
    for(int t=0; t<PIXTIME + FIREBALLS + BOOMBALLS + 1 ; t++)
    {
      imgCash[c][t] = new QPixmap(CELLSIZE, CELLSIZE);
      QPainter p(imgCash[c][t]);
      p.drawPixmap(0,0,(*backgroundPix),0,0,CELLSIZE,CELLSIZE);
      p.drawPixmap(1,1,(*balls),t*PIXSIZE,c*PIXSIZE,PIXSIZE,PIXSIZE);
    }
    for(int t=0; t < FIREPIX ; t++)
    {
      firePix[t] = new QPixmap(CELLSIZE, CELLSIZE);
      QPainter p(firePix[t]);
      p.drawPixmap(0,0,(*backgroundPix),0,0,CELLSIZE,CELLSIZE);
      p.drawPixmap(1,1,(*fire),t*PIXSIZE,0,PIXSIZE,PIXSIZE);
    }
  }
  delete balls;
  delete fire;
}


QPixmap* BallPainter::GetBall(int color, int animstep, int panim)
{
//    return backgroundPix;

    if( (color<0) || (color>=NCOLORS) || (animstep<0) || color == NOBALL ){
       return backgroundPix;
    }
    if ( panim == ANIM_JUMP )
    {
      if ( ( animstep < 0 ) || ( animstep >= PIXTIME ) )
        return backgroundPix;
      else
        return imgCash[color][animstep];
    }
    else if ( panim == ANIM_BURN )
    {
        if ( animstep < FIREBALLS )
          return imgCash[color][animstep + PIXTIME + BOOMBALLS + 1];
        else if ( animstep < FIREBALLS + FIREPIX )
          return firePix[animstep - FIREBALLS];
    }
    else if ( panim == ANIM_BORN )
    {
        if ( animstep < BOOMBALLS )
          return imgCash[color][animstep + PIXTIME];
        else
          return imgCash[color][NORMALBALL];
    }
    // rest is not imlemented yet
    return imgCash[color][NORMALBALL];

}
