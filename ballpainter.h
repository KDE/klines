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

#ifndef BALLPAINTER_H
#define BALLPAINTER_H

#include <qobject.h>
#include <qpixmap.h>
#include "cfg.h"

#define CELLSIZE 32

class BallPainter : public QObject
{
  Q_OBJECT
  QPixmap* imgCash[NCOLORS][PIXTIME + FIREBALLS + BOOMBALLS + 1];
  QPixmap* backgroundPix;
  QPixmap* firePix[FIREPIX];


public:
  BallPainter();
  ~BallPainter();
  
  void deletePix();
  void createPix();

  QPixmap GetBall( int color, int animstep, int panim );
  QPixmap GetNormalBall(int color) { return GetBall(color,0,ANIM_NO); }
  QPixmap GetBackgroundPix() { return GetBall(NOBALL,0,ANIM_NO); }
};

#endif
