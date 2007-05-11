/***************************************************************************
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
    copyright            : (C) 2006 by Dmitry Suzdalev
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
#include "preview.h"
#include "scene.h"

#include <KLocale>

#include <QLabel>
#include <QLayout>

MainWidget::MainWidget( QWidget* parent )
    : QWidget( parent )
{
    QBoxLayout *mainLay = new QHBoxLayout( this );
    mainLay->setMargin( 0 );

    m_scene = new KLinesScene(this);
    KLinesView* klview = new KLinesView( m_scene, this );
    klview->setCacheMode( QGraphicsView::CacheBackground );
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

#include "mwidget.moc"
