/***************************************************************************
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
#include <kmessagebox.h>
//#include "shotcounter.h"
#include <qpainter.h>
#include "linesboard.h"
//#include <qcolor.h>
#include <qjpegio.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <stdlib.h>

#include "prefs.h"

#define PIXSIZE (CELLSIZE - 2)

int colorLinesArr[NCOLORS] =
    {0x0000ff, 0x00ff00, 0xff0000, 0x00ffff, 0xff00ff, 0xffff00, 0x005080};
    // 0x00bbggrr
    // red   ,  green  ,  blue   , yellow  , violet  ,   cyan  , brown



BallPainter::BallPainter()
    : QObject(), backgroundPix(0)
{
  createPix();
}

BallPainter::~BallPainter()
{
  deletePix();
}

void BallPainter::deletePix()
{
  delete backgroundPix;
  for(int c=0; c<NCOLORS; c++)
    for(int t=0; t<PIXTIME + FIREBALLS + BOOMBALLS + 1 ; t++)
      delete imgCash[c][t];
  for(int t=0; t < FIREPIX ; t++)
    delete firePix[t];
}

void BallPainter::createPix()
{
  backgroundPix = new QPixmap(
		locate( "appdata", "field.jpg" ));
  QPixmap *balls = new QPixmap(
		locate( "appdata", "balls.jpg" ));
  QPixmap *fire = new QPixmap(
		locate( "appdata", "fire.jpg" ));
  if (balls->isNull() ||backgroundPix->isNull() || fire->isNull() ) {
      KMessageBox::error(0, i18n("Unable to find graphics. Check your installation."), i18n("Error"));
      qApp->exit(1);
      return; // Error
  }

  for(int c=0; c<NCOLORS; c++)
  {
    for(int t=0; t<PIXTIME + FIREBALLS + BOOMBALLS + 1 ; t++)
    {
      imgCash[c][t] = new QPixmap(CELLSIZE, CELLSIZE);
      QPainter p(imgCash[c][t]);
      p.drawPixmap(0,0,(*backgroundPix),0,0,CELLSIZE,CELLSIZE);
      p.drawPixmap(1,1,(*balls),t*PIXSIZE,c*PIXSIZE,PIXSIZE,PIXSIZE);
      if (Prefs::numberedBalls() && (t == NORMALBALL))
      {
        if ((c == 2) || (c == 3) || (c == 6))
          p.setPen(Qt::black);
        else
          p.setPen(Qt::white);
        QString tmp;
        tmp.setNum(c+1);
        p.drawText(QRect(0,0,CELLSIZE,CELLSIZE), Qt::AlignCenter, tmp);
      }
      
    }
  }
  for(int t=0; t < FIREPIX ; t++)
  {
    firePix[t] = new QPixmap(CELLSIZE, CELLSIZE);
    QPainter p(firePix[t]);
    p.drawPixmap(0,0,(*backgroundPix),0,0,CELLSIZE,CELLSIZE);
    p.drawPixmap(1,1,(*fire),t*PIXSIZE,0,PIXSIZE,PIXSIZE);
  }
  delete balls;
  delete fire;
}


QPixmap BallPainter::GetBall(int color, int animstep, int panim)
{
//    return backgroundPix;

    if( (color<0) || (color>=NCOLORS) || (animstep<0) || color == NOBALL ){
       return *backgroundPix;
    }
    if ( panim == ANIM_JUMP )
    {
      if ( ( animstep < 0 ) || ( animstep >= PIXTIME ) )
        return *backgroundPix;
      else
        return *imgCash[color][animstep];
    }
    else if ( panim == ANIM_BURN )
    {
        if ( animstep < FIREBALLS )
          return *imgCash[color][animstep + PIXTIME + BOOMBALLS + 1];
        else if ( animstep < FIREBALLS + FIREPIX )
          return *firePix[animstep - FIREBALLS];
    }
    else if ( panim == ANIM_BORN )
    {
        if ( animstep < BOOMBALLS )
          return *imgCash[color][animstep + PIXTIME];
        else
          return *imgCash[color][NORMALBALL];
    }
    // rest is not imlemented yet
    return *imgCash[color][NORMALBALL];

}

#include "ballpainter.moc"
