/***************************************************************************
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
    copyright            : (C) 2006-2007 by Dmitry Suzdalev
    email                : dimsuz@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "mwidget.h"
#include "scene.h"

#include <KLocale>

#include <QGraphicsView>
#include <QLabel>
#include <QLayout>
#include <QResizeEvent>

MainWidget::MainWidget( QWidget* parent )
    : QWidget( parent )
{
    QBoxLayout *mainLay = new QHBoxLayout( this );
    mainLay->setMargin( 0 );

    m_scene = new KLinesScene(this);
    QGraphicsView* klview = new QGraphicsView( m_scene, this );
    klview->setCacheMode( QGraphicsView::CacheBackground );
    klview->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    klview->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    klview->setFrameStyle(QFrame::NoFrame);

    klview->setOptimizationFlags( QGraphicsView::DontClipPainter |
                                  QGraphicsView::DontSavePainterState |
                                  QGraphicsView::DontAdjustForAntialiasing );

    mainLay->addWidget( klview );
}

MainWidget::~MainWidget()
{
}

void MainWidget::setShowNextColors(bool visible)
{
    // add bonus score points if playing w/o preview
    m_scene->setBonusScorePoints( visible ? 0 : 1 );
    m_scene->setPreviewZoneVisible( visible );
}

void MainWidget::resizeEvent( QResizeEvent* ev)
{
    // if this flag is set it means that resizeEvent is called
    // while widget is hidden.
    // so we'll wait with resize until it will be unset to not
    // waste time resizing invisible scene
    if ( !testAttribute( Qt::WA_PendingResizeEvent ) )
        m_scene->resizeScene( ev->size().width(), ev->size().height() );
}

#include "mwidget.moc"
