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
#include <QGraphicsSceneMouseEvent>

#include <kdebug.h>

#include "scene.h"
#include "ballitem.h"
#include "animator.h"
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
    m_animator = new KLinesAnimator(this);
    connect( m_animator, SIGNAL(moveFinished()), SLOT(moveAnimFinished() ) );

    for(int x=0; x<FIELD_SIZE; ++x)
        for(int y=0; y<FIELD_SIZE; ++y)
            m_field[x][y] = 0;
    
    nextThreeBalls();
}

KLinesScene::~KLinesScene()
{
    delete m_renderer;
    delete m_animator;
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
}

void KLinesScene::placeRandomBall()
{
    // FIXME dimsuz: in old klines ball positon had score and levels of
    // difficulty were implemented around it. Check this out and consider implementing
    // as current pos finding isn't very good - theorectically it can search forever :).
    // @see linesboard.cpp: placeBall()
    int posx = -1, posy = -1;
    // let's find random free cell
    do
    {
        posx = m_randomSeq.getLong(FIELD_SIZE);
        posy = m_randomSeq.getLong(FIELD_SIZE);
    } while( m_field[posx][posy] != 0 );

    // random color
    BallColor c = static_cast<BallColor>(m_randomSeq.getLong(static_cast<int>(NumColors)));

    BallItem* newBall = new BallItem( this, m_renderer );
    newBall->setColor(c);
    newBall->setPos( fieldToPix( FieldPos(posx,posy) ) );
    m_field[posx][posy] = newBall;

    newBall->startAnimation( BornAnimation );
    m_numBalls++;
}

void KLinesScene::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    if( m_animator->isAnimating() )
        return;

    FieldPos fpos = pixToField(ev->scenePos());

    if( m_field[fpos.x][fpos.y] ) // ball was selected
    {
        if( m_selPos.isValid() )
            m_field[m_selPos.x][m_selPos.y]->stopAnimation();

        m_field[fpos.x][fpos.y]->startAnimation( SelectedAnimation );
        m_selPos = fpos;
    }
    else // move selected ball to new location
    {
        if( m_selPos.isValid() && m_field[fpos.x][fpos.y] == 0 )
        {
            // FIXME dimsuz: check if we have a valid path to desired point
            BallItem *selectedBall = m_field[m_selPos.x][m_selPos.y];
            selectedBall->stopAnimation();

            // start move animation
            m_animator->animateMove(m_selPos, fpos);
        }
    }
}

void KLinesScene::moveAnimFinished()
{
    // m_field[m_selPos.x][m_selPos.y] still holds the ball pointer
    // but animation placed it to new location.
    // But it updated only pixel position, not the field one
    // So let's do it here
    BallItem *movedBall = m_field[m_selPos.x][m_selPos.y];
    // movedBall has new pixel position - let's find out corresponding field pos
    FieldPos newpos = pixToField(movedBall->pos());

    m_field[m_selPos.x][m_selPos.y] = 0; // no more ball here
    m_field[newpos.x][newpos.y] = movedBall;

    m_selPos.x = m_selPos.y = -1; // invalidate position

    searchAndErase();

    nextThreeBalls();

    searchAndErase();
}

void KLinesScene::searchAndErase()
{
    // FIXME dimsuz: put more comments about bounds in for loops
    
    // horizontal chunks searching
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=0;y<FIELD_SIZE; ++y)
        {
            if(m_field[x][y] == 0)
                continue;

            BallColor col = m_field[x][y]->color();
            int tmpx = x+1;
            while(tmpx < FIELD_SIZE && m_field[tmpx][y] && m_field[tmpx][y]->color() == col)
                tmpx++;
            // tmpx-x will be: how much balls of the same color we found
            if(tmpx-x >= 5)
            {
                for(int i=x; i<tmpx;++i)
                {
                    delete m_field[i][y];
                    m_field[i][y] = 0;
                }
            }
            else
                continue;
        }

    // vertical chunks searching
    for(int y=0; y<FIELD_SIZE-4; ++y)
        for(int x=0;x<FIELD_SIZE; ++x)
        {
            if(m_field[x][y] == 0)
                continue;

            BallColor col = m_field[x][y]->color();
            int tmpy = y+1;
            while(tmpy < FIELD_SIZE && m_field[x][tmpy] && m_field[x][tmpy]->color() == col)
                tmpy++;
            // tmpy-y will be: how much balls of the same color we found
            if(tmpy-y >= 5)
            {
                for(int j=y; j<tmpy;++j)
                {
                    delete m_field[x][j];
                    m_field[x][j] = 0;
                }
            }
            else
                continue;
        }

    // down-right diagonal
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=0;y<FIELD_SIZE-4; ++y)
        {
            if(m_field[x][y] == 0)
                continue;

            BallColor col = m_field[x][y]->color();
            int tmpx = x+1;
            int tmpy = y+1;
            while(tmpx < FIELD_SIZE && tmpy < FIELD_SIZE && 
                    m_field[tmpx][tmpy] && m_field[tmpx][tmpy]->color() == col)
            {
                tmpx++;
                tmpy++;
            }
            // tmpx-x (and tmpy-y too) will be: how much balls of the same color we found
            if(tmpx-x >= 5)
            {
                for(int i=x,j=y; i<tmpx;++i,++j)
                {
                    delete m_field[i][j];
                    m_field[i][j] = 0;
                }
            }
            else
                continue;
        }

    // up-right diagonal
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=5; y<FIELD_SIZE; ++y)
        {
            if(m_field[x][y] == 0)
                continue;

            BallColor col = m_field[x][y]->color();
            int tmpx = x+1;
            int tmpy = y-1;
            while(tmpx < FIELD_SIZE && tmpy >=0 && 
                    m_field[tmpx][tmpy] && m_field[tmpx][tmpy]->color() == col)
            {
                tmpx++;
                tmpy--;
            }
            // tmpx-x (and tmpy-y too) will be: how much balls of the same color we found
            if(tmpx-x >= 5)
            {
                for(int i=x,j=y; i<tmpx;++i,--j)
                {
                    delete m_field[i][j];
                    m_field[i][j] = 0;
                }
            }
            else
                continue;
        }
}

void KLinesScene::drawBackground(QPainter *p, const QRectF&)
{
    kDebug() << k_funcinfo << endl;
    // FIXME dimsuz: temp
    for(int x=0; x<32*FIELD_SIZE;x+=32)
        for(int y=0; y<32*FIELD_SIZE;y+=32)
            p->drawPixmap( x, y, m_renderer->backgroundTilePixmap() );
}

#include "scene.moc"
