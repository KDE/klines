/***************************************************************************
                          mwidget.cpp  -  description
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
#include "mwidget.moc"
#include <qlayout.h>
#include "ballpainter.h"

MainWidget::MainWidget( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
		QBoxLayout *grid = new QHBoxLayout( this, 5 );     //(rows,col)
    BallPainter * bPainter = new BallPainter();

    lsb = new LinesBoard(bPainter, this);
    connect(lsb, SIGNAL(endTurn()), parent, SLOT(makeTurn()));
    connect(lsb, SIGNAL(eraseLine(int)), parent, SLOT(addScore(int)));
    connect(lsb, SIGNAL(endGame()), parent, SLOT(stopGame()));
    grid->addWidget( lsb );


    lPrompt = new LinesPrompt(bPainter, this);
    connect(lPrompt, SIGNAL(PromptPressed()), parent, SLOT(switchPrompt()));

    grid->addWidget( lPrompt );

    grid->activate();
    grid->freeze(0,0);

//    warning("width: %i height: %i", width(), height() );

//    warning("wh: %i hh: %i", sizeHint().width(), sizeHint().height() );

}

/*
   Destructor: deallocates memory for contents
*/

MainWidget::~MainWidget()
{
}

LinesBoard * MainWidget::GetLsb()
{
    return lsb;
}

LinesPrompt * MainWidget::GetPrompt()
{
    return lPrompt;
}
