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

    QBoxLayout *right = new QVBoxLayout;
    mainLay->addLayout(right);
    right->setMargin(2);
    m_next_label = new QLabel(i18n("Next:"), this);
    m_next_label->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    m_next_label->setAlignment( Qt::AlignHCenter );

    m_preview = new BallsPreview(this);
    updateNextColors();

    right->addStretch( 1 );
    right->addWidget( m_next_label );
    right->addWidget( m_preview );
    right->addStretch( 1 );

    mainLay->addSpacing( mainLay->spacing() );

    connect(m_scene, SIGNAL(nextColorsChanged()), SLOT(updateNextColors()) );
}

MainWidget::~MainWidget()
{
}

void MainWidget::resizeEvent( QResizeEvent* )
{
    m_preview->updateGeometry();
}

void MainWidget::updateNextColors()
{
    m_preview->setColors( m_scene->nextColors() );
}

void MainWidget::setShowNextColors(bool visible)
{
    m_preview->setVisible( visible );
    m_next_label->setVisible( visible );
    // add bonus score points if playing w/o preview
    m_scene->setBonusScorePoints( visible ? 0 : 1 );
}

#include "mwidget.moc"
