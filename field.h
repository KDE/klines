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

#ifndef FIELD_H
#define FIELD_H

#include <qobject.h>
#include <qwidget.h>
#include "cell.h"
// size of game field
#define NUMCELLSW 9
#define NUMCELLSH 9

class Field: public QWidget
{
  Q_OBJECT
public:
  void clearField();

  int calcPosScore(int x, int y, int whatIf);
  void saveUndo();

protected:
  Field(QWidget* parent, const char* name);
  ~Field();

  void putBall(int x, int y, int color);
  void putBallRun(int x, int y, int color);
  void removeBall(int x, int y );
  int getBall(int x, int y);
  int getAnim(int x, int y);
  void setAnim(int x, int y, int anim );
  void clearAnim();
  int deleteAnimatedBalls();
  void moveBall(int xa, int ya, int xb, int yb);
  int calcRun(int sx, int sy, int dx, int dy);
  

  bool checkBounds( int x, int y );
//  virtual int erase5Balls(){ return 0;}
  int freeSpace();
  void restoreUndo();

private:
  Cell field[NUMCELLSH][NUMCELLSW];
  Cell field_undo[NUMCELLSH][NUMCELLSW];
//  void search5Balls();

};

#endif
