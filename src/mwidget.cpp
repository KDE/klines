/*
    SPDX-FileCopyrightText: 2000 Roman Merzlyakov <roman@sbrf.barrt.ru>
    SPDX-FileCopyrightText: 2000 Roman Razilov <Roman.Razilov@gmx.de>
    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mwidget.h"
#include "scene.h"


#include <QGraphicsView>
#include <QResizeEvent>
#include <QBoxLayout>

MainWidget::MainWidget( QWidget* parent )
    : QWidget( parent )
    , m_scene( nullptr )
    , m_next_label( nullptr )
{
    QBoxLayout *mainLay = new QHBoxLayout( this );
    mainLay->setContentsMargins( 0 ,  0 ,  0 ,  0 );

    m_scene = new KLinesScene(this);
    QGraphicsView* klview = new QGraphicsView( m_scene, this );
    klview->setCacheMode( QGraphicsView::CacheBackground );
    klview->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    klview->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    klview->setFrameStyle(QFrame::NoFrame);

    klview->setOptimizationFlags( 
                                  QGraphicsView::DontSavePainterState |
                                  QGraphicsView::DontAdjustForAntialiasing );

    mainLay->addWidget( klview );

    setMinimumSize( 250, 250 );
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
        m_scene->resizeScene( ev->size().width(), ev->size().height() );
}


