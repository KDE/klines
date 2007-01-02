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
#include <klocale.h>

#include <QLabel>
#include <QLayout>

#include "mwidget.h"
#include "ballpainter.h"
#include "linesboard.h"
#include "prompt.h"
#include "preview.h"

#include "scene.h"

MainWidget::MainWidget( QWidget* parent )
    : QFrame( parent )
{
    QBoxLayout *grid = new QHBoxLayout( this );     //(rows,col)
    grid->setMargin( 5 );
    bPainter = new BallPainter();

    lsb = new LinesBoard(bPainter, this);
    // FIXME dimsuz: remove lsb
    lsb->hide();
//    grid->addWidget( lsb );

    m_scene = new KLinesScene(this);
    KLinesView* klview = new KLinesView( m_scene, this );
    klview->setCacheMode( QGraphicsView::CacheBackground );
    grid->addWidget( klview );

    QBoxLayout *right = new QVBoxLayout;
    grid->addLayout(right);
    right->setMargin(2);
    QLabel *label = new QLabel(i18n("Next balls:"), this);
    lPrompt = new LinesPrompt(bPainter, this);
    connect(lPrompt, SIGNAL(PromptPressed()), parent, SLOT(switchPrompt()));

    m_preview = new BallsPreview(this);
    updateNextColors();

    right->addWidget( label, 0, Qt::AlignBottom | Qt::AlignHCenter );
    // FIXME dimsuz: remove lPrompt
    lPrompt->hide();
    right->addWidget( m_preview );
    //right->addWidget( lPrompt, 0, Qt::AlignTop | Qt::AlignHCenter );

    connect(m_scene, SIGNAL(nextColorsChanged()), SLOT(updateNextColors()) );

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

void MainWidget::updateNextColors() 
{
    m_preview->setColors( m_scene->nextColors() );
}

#include "mwidget.moc"
