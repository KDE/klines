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

#include "mwidget.moc"

#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>

#include "ballpainter.h"

MainWidget::MainWidget( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    QBoxLayout *grid = new QHBoxLayout( this, 5 );     //(rows,col)
    bPainter = new BallPainter();

    lsb = new LinesBoard(bPainter, this);
    grid->addWidget( lsb );

    QBoxLayout *right = new QVBoxLayout(grid, 2);
    QLabel *label = new QLabel(i18n("Next balls:"), this);
    lPrompt = new LinesPrompt(bPainter, this);
    connect(lPrompt, SIGNAL(PromptPressed()), parent, SLOT(switchPrompt()));

    right->addWidget( label, 0, Qt::AlignBottom | Qt::AlignHCenter );
    right->addWidget( lPrompt, 0, Qt::AlignTop | Qt::AlignHCenter );

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

void MainWidget::updatePix()
{
    bPainter->deletePix();
    bPainter->createPix();
    lPrompt->update();
    lsb->update();
}
