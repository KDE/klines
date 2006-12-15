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

#include <qpainter.h>
#include "prompt.h"
#include "prompt.moc"
#include "ballpainter.h"

LinesPrompt::LinesPrompt( BallPainter * abPainter, QWidget* parent )
    : QWidget( parent )
{

  bPainter = abPainter;

  setFocusPolicy( Qt::NoFocus );
  QPalette palette;
  palette.setColor( backgroundRole(), Qt::gray );
  setPalette( palette );

  setMouseTracking( false );
  setFixedSize(wPrompt(), hPrompt());

  PromptEnabled = true;
  cb[0]=NOBALL;
  cb[1]=NOBALL;
  cb[2]=NOBALL;
}

LinesPrompt::~LinesPrompt()
{
}

void LinesPrompt::paintEvent( QPaintEvent* )
{
  QPainter paint( this );
  if(PromptEnabled){
    paint.drawPixmap(0,          0, bPainter->GetNormalBall(cb[0]) );
    paint.drawPixmap(CELLSIZE,   0, bPainter->GetNormalBall(cb[1]) );
    paint.drawPixmap(2*CELLSIZE, 0, bPainter->GetNormalBall(cb[2]) );
  }
  else{
    paint.drawPixmap(0,          0, bPainter->GetBackgroundPix() );
    paint.drawPixmap(CELLSIZE,   0, bPainter->GetBackgroundPix() );
    paint.drawPixmap(2*CELLSIZE, 0, bPainter->GetBackgroundPix() );
  }
}

/*
   Handles mouse press events for the LinesPrompt widget.
*/
void LinesPrompt::mousePressEvent( QMouseEvent*  )
{
    emit PromptPressed();
}

void LinesPrompt::SetBalls( int *pcb )
{
		for (int i = 0; i<BALLSDROP; i++)
			cb[i] = pcb[i];

    update();
}

bool LinesPrompt::getState()
{
    return PromptEnabled;
}
void LinesPrompt::setPrompt(bool enabled)
{
    PromptEnabled = enabled;
    update();
}
