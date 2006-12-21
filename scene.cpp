/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KLines"
 *
 * KLines is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KLines is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KAtomic; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#include <QResizeEvent>

#include <kdebug.h>

#include "scene.h"
#include "ballitem.h"
#include "renderer.h"

KLinesView::KLinesView( KLinesScene* scene, QWidget* parent )
    : QGraphicsView(scene, parent)
{
    //setMinimumSize( defaultFieldSize, defaultFieldSize );
}

void KLinesView::resizeEvent( QResizeEvent* ev )
{
    static_cast<KLinesScene*>(scene())->resizeScene( ev->size().width(), ev->size().height() );
}

// =============== KLinesScene =======================

KLinesScene::KLinesScene( QObject* parent )
    : QGraphicsScene(parent), m_numBalls(0)
{
    m_renderer = new KLinesRenderer;
    nextThreeBalls();

    for(int x=0; x<FIELD_SIZE; ++x)
        for(int y=0; y<FIELD_SIZE; ++y)
            m_field[x][y] = 0;
}

KLinesScene::~KLinesScene()
{
    delete m_renderer;
}

void KLinesScene::resizeScene(int width,int height)
{
    kDebug() << "resize:" << width << "," << height << endl;
    setSceneRect( 0, 0, width, height );
}

void KLinesScene::nextThreeBalls()
{
    placeRandomBall();
    placeRandomBall();
    placeRandomBall();
    kDebug() << "=======" << endl;
}

void KLinesScene::placeRandomBall()
{
    int posx = -1, posy = -1;
    // let's find random free cell
    do
    {
        // FIXME dimsuz: debug temp
        if( posx != -1 )
            kDebug() << "pos x:" << posx << " y:" << posy << " isn't free!" << endl;
        posx = m_randomSeq.getLong(FIELD_SIZE);
        posy = m_randomSeq.getLong(FIELD_SIZE);
        kDebug() << "trying x:" << posx << " y:" << posy << endl;
    } while( m_field[posx][posy] != 0 );

    kDebug() << "found x:" << posx << " y:" << posy << endl;
    // random color
    BallColor c = static_cast<BallColor>(m_randomSeq.getLong(static_cast<int>(NumColors)));

    BallItem* newBall = new BallItem( this, m_renderer );
    newBall->setColor(c);
    newBall->setPos( fieldToPix(posx,posy) );
    m_field[posx][posy] = newBall;

    //newBall->startAnimation( BornAnimation );
    newBall->startAnimation( SelectedAnimation );
    m_numBalls++;
}

void KLinesScene::mouseReleaseEvent( QGraphicsSceneMouseEvent* )
{
    nextThreeBalls();
}

void KLinesScene::drawBackground(QPainter *p, const QRectF&)
{
    kDebug() << k_funcinfo << endl;
    // FIXME dimsuz: temp
    for(int x=0; x<32*FIELD_SIZE;x+=32)
        for(int y=0; y<32*FIELD_SIZE;y+=32)
            p->drawPixmap( x, y, m_renderer->backgroundTilePixmap() );
}
