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
#include <KLocale>

#include <QLabel>
#include <QLayout>

#include "mwidget.h"
#include "preview.h"
#include "scene.h"

MainWidget::MainWidget( QWidget* parent )
    : QWidget( parent )
{
    QBoxLayout *mainLay = new QHBoxLayout( this );
    mainLay->setMargin( 5 );

    m_scene = new KLinesScene(this);
    KLinesView* klview = new KLinesView( m_scene, this );
    klview->setCacheMode( QGraphicsView::CacheBackground );
    mainLay->addWidget( klview );

    QBoxLayout *right = new QVBoxLayout;
    mainLay->addLayout(right);
    right->setMargin(2);
    QLabel *label = new QLabel(i18n("Next balls:"), this);

    m_preview = new BallsPreview(this);
    updateNextColors();

    right->addWidget( label );
    right->addWidget( m_preview );

    connect(m_scene, SIGNAL(nextColorsChanged()), SLOT(updateNextColors()) );
}

MainWidget::~MainWidget()
{
}

void MainWidget::updateNextColors() 
{
    m_preview->setColors( m_scene->nextColors() );
}

void MainWidget::setShowNextColors(bool visible)
{
    m_preview->setShowColors(visible);
}

#include "mwidget.moc"
