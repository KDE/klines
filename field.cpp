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

#include <stdlib.h>
#include "cfg.h"
#include "field.moc"

Field::Field(QWidget* parent, const char* name)
    : QWidget( parent, name )
{
  clearField();
}

Field::~Field()
{
}

void Field::clearField()
{
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
      field[y][x].clear();
}
void Field::clearAnim()
{
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
      field[y][x].setAnim( ANIM_NO );
}
int Field::deleteAnimatedBalls()
{
  int deleted = 0;
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
    {
      if ( field[y][x].getAnim() != ANIM_NO )
      {
        deleted++;
        field[y][x].clear();
      }
    }
  return deleted;
}

bool Field::checkBounds(int x, int y)
{
  return (x>=0) && (x<NUMCELLSW) && (y>=0) && (y<NUMCELLSH);
}

void Field::putBall(int x, int y, int color)
{
    if( checkBounds(x,y) ){
      field[y][x].setColor(color);
    }
}
/*
void Field::putBall(int x, int y, char color)
{
    if( checkBounds(x,y) ){
      field[y][x].setColor(color);
      erase5Balls();
      repaint(FALSE);
    }
}*/
void Field::moveBall(int xa, int ya, int xb, int yb)
{
    if( checkBounds(xa,ya) && checkBounds(xb,yb) &&
      field[yb][xb].isFree() && ( xa != xb || ya != yb) ) {
      field[yb][xb].moveBall(field[ya][xa]);
    }
}

int Field::getBall(int x, int y)
{
    if( checkBounds(x,y) )
    	return field[y][x].getColor();
    else 
    	return NOBALL;
}
int Field::getAnim(int x, int y)
{
    if( checkBounds(x,y) )
    	return field[y][x].getAnim();
    else 
    	return NOBALL;
}
void Field::setAnim(int x, int y, int anim)
{
    if( checkBounds(x,y) )
    	field[y][x].setAnim( anim );
}

void Field::removeBall(int x, int y )
{
    if( checkBounds(x,y) ) field[y][x].clear();
}

int Field::freeSpace()
{
    int s = 0;
    for(int y=0; y<NUMCELLSH; y++)
      for(int x=0; x<NUMCELLSW; x++)
        if(field[y][x].isFree()) s++;
    return s;
}
void Field::saveUndo()
{
  void clearAnim();
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
      field_undo[y][x] = field[y][x];
}
void Field::restoreUndo()
{
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
      field[y][x] = field_undo[y][x];
}

int Field::calcRun(int sx, int sy, int dx, int dy)
{
  int ix = sx;
  int iy = sy;
  int run = 0;
  int score = 0;
  int current_color = NOBALL;
  int lpr = 0;
  int lprscore = 0;
  int empty = 0;
  for(int i = 0; i < 9; i++, ix += dx, iy += dy)
  {
    if ((ix < 0) || (iy < 0) || (ix >= NUMCELLSW) || (iy >= NUMCELLSH))
       continue;
    int b = field[iy][ix].getColor();
//printf("[%d,%d]%d", ix,iy,b);
    if (b == NOBALL)
    {
       run++;
       empty++;
    }
    else if (b == current_color)
    {
       run++;
       score++;
       empty = 0;
    }
    else
    {
       run = empty+1;
       score = 1;
       current_color = b;
       empty = 0;
    }
    if (run > lpr)
    {
       lpr = run;
       lprscore = score;
    }
  }
  if (lpr < 5)
    lprscore = -10;
  else 
    lprscore = lprscore*lprscore;
//  printf("= %d\n", lprscore); 
  return lprscore;
}

int Field::calcPosScore(int x, int y, int whatIf)
{
   int score = -10;
   int color = field[y][x].getColor();
   field[y][x].setColor(whatIf);
   score = QMAX(score, calcRun(x, y-4, 0, 1));
   score = QMAX(score, calcRun(x-4, y-4, 1, 1));
   score = QMAX(score, calcRun(x-4, y, 1, 0));
   score = QMAX(score, calcRun(x-4, y+4, 1, -1));
   field[y][x].setColor(color);
   return score;
}

