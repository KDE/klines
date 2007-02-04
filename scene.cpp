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
    : QGraphicsScene(parent), m_numFreeCells(FIELD_SIZE*FIELD_SIZE), m_score(0), m_bonusScore(0), m_cellSize(32)
{
    m_animator = new KLinesAnimator(this);
    connect( m_animator, SIGNAL(moveFinished()), SLOT(moveAnimFinished() ) );
    connect( m_animator, SIGNAL(removeFinished()), SLOT(removeAnimFinished() ) );
    connect( m_animator, SIGNAL(bornFinished()), SLOT(bornAnimFinished() ) );

    m_focusItem = new QGraphicsRectItem( QRectF(0, 0, m_cellSize, m_cellSize), 0, this );
    m_focusItem->setZValue(1.0);
    m_focusItem->setPen( Qt::DashLine );

    startNewGame();
}

void KLinesScene::startNewGame()
{
    // reset all vars
    m_selPos = FieldPos();
    m_numFreeCells = FIELD_SIZE*FIELD_SIZE;
    m_score = 0;
    m_bonusScore = 0;
    m_placeBalls = true;
    m_itemsToDelete.clear();
    m_nextColors.clear();
    m_focusItem->setPos(0, 0);
    m_focusItem->hide();

    // remove all ball items from the scene
    QList<QGraphicsItem*> itemlist = items();
    foreach( QGraphicsItem* item, itemlist )
    {
        if( item != m_focusItem )
        {
            removeItem(item);
            delete item;
        }
    }

    for(int x=0; x<FIELD_SIZE; ++x)
        for(int y=0; y<FIELD_SIZE; ++y)
            m_field[x][y] = 0;

    // init m_nextColors
    for(int i=0; i<3; i++)
    {
        // random color
        BallColor c = static_cast<BallColor>(m_randomSeq.getLong(static_cast<int>(NumColors)));
        m_nextColors.append(c);
    }

    emit enableUndo(false);

    nextThreeBalls();
}

KLinesScene::~KLinesScene()
{
    delete m_animator;
}

void KLinesScene::resizeScene(int width,int height)
{
    // store focus item field pos (calculated using old cellSize)
    FieldPos focusRectFieldPos = pixToField( m_focusItem->pos() );

    m_cellSize = qMin(width, height)/FIELD_SIZE;
    setSceneRect( 0, 0, width, height );

    KLinesRenderer::self()->setCellSize( m_cellSize );

    // re-render && recalc positions for all balls
    for( int x=0; x<FIELD_SIZE; ++x)
        for(int y=0; y< FIELD_SIZE; ++y)
        {
            if( m_field[x][y] )
            {
                // this will update pixmap
                m_field[x][y]->setColor( m_field[x][y]->color() );
                m_field[x][y]->setPos( fieldToPix( FieldPos(x,y) ) );
            }
        }

    m_focusItem->setRect( QRect(0,0, m_cellSize, m_cellSize) );
    m_focusItem->setPos( fieldToPix( focusRectFieldPos ) );

    kDebug() << "resize:" << width << "," << height << "; cellSize: " << m_cellSize << endl;
}

void KLinesScene::endTurn()
{
    saveUndoInfo();
    nextThreeBalls();
}

void KLinesScene::nextThreeBalls()
{
    if( m_animator->isAnimating() )
        return;

    QList<BallItem*> newItems;
    BallItem* newBall;
    for(int i=0; i<3; i++)
    {
        newBall = randomlyPlaceBall( m_nextColors.at(i) );
        if( newBall )
            newItems.append(newBall);
        else
            break; // the field is filled :).
    }

    for(int i=0; i<3; i++)
    {
        // random color
        BallColor c = static_cast<BallColor>(m_randomSeq.getLong(static_cast<int>(NumColors)));
        m_nextColors[i] = c;
    }

    emit nextColorsChanged();

    m_animator->animateBorn( newItems );
}

BallItem* KLinesScene::randomlyPlaceBall(BallColor c)
{
    m_numFreeCells--;
    if(m_numFreeCells < 0)
        return 0; // game over, we won't create more balls

    int posx = -1, posy = -1;
    // let's find random free cell
    do
    {
        posx = m_randomSeq.getLong(FIELD_SIZE);
        posy = m_randomSeq.getLong(FIELD_SIZE);
    } while( m_field[posx][posy] != 0 );

    BallItem* newBall = new BallItem( this );
    newBall->setColor(c, false); // pixmap will be set by born animation
    newBall->setPos( fieldToPix( FieldPos(posx,posy) ) );
    m_field[posx][posy] = newBall;
    return newBall;
}

void KLinesScene::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    selectOrMove( pixToField(ev->scenePos()) );
}

void KLinesScene::selectOrMove( const FieldPos& fpos )
{
    if( m_animator->isAnimating() )
        return;

    if( m_field[fpos.x][fpos.y] ) // ball was selected
    {
        if( m_selPos.isValid() )
            m_field[m_selPos.x][m_selPos.y]->stopAnimation();

        m_field[fpos.x][fpos.y]->startSelectedAnimation();
        m_selPos = fpos;
    }
    else // move selected ball to new location
    {
        if( m_selPos.isValid() && m_field[fpos.x][fpos.y] == 0 )
        {
            saveUndoInfo();
            // start move animation
            // slot moveAnimFinished() will be called when it finishes
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

    m_placeBalls = true;
    // after anim finished, slot removeAnimFinished()
    // will be called
    searchAndErase();
}

void KLinesScene::removeAnimFinished()
{
    if( m_itemsToDelete.isEmpty() && m_numFreeCells == 0 )
    {
        emit enableUndo(false);
        emit gameOver(m_score);
        return;
    }

    if(m_itemsToDelete.isEmpty() && m_placeBalls)
    {
        // slot bornAnimFinished() will be called
        // when born animation finishes
        // NOTE: removeAnimFinished() will be called again
        // after new balls will born (because searchAndErase() will be called)
        // but other if branch will be taken
        nextThreeBalls();
    }
    else
    {
        // this is kind of 'things to do after one turn is finished'
        // place in code :)

        int numBallsErased = m_itemsToDelete.count();
        if(numBallsErased)
        {
            // expression taked from previous code in klines.cpp
            m_score += 2*numBallsErased*numBallsErased - 20*numBallsErased + 60 ;
            m_score += m_bonusScore;
        }

        foreach( BallItem* item, m_itemsToDelete )
        {
            removeItem(item);
            delete item;
        }
        m_itemsToDelete.clear();

        if(numBallsErased)
            emit scoreChanged(m_score);
    }

}

void KLinesScene::bornAnimFinished()
{
    // note that if m_numFreeCells == 0, we still need to
    // check for possible 5-in-a-row balls, i.e. call searchAndErase()
    // So there's another gameOver()-check in removeAnimFinished()
    if( m_numFreeCells < 0 )
    {
        kDebug() << "GAME OVER" << endl;
        emit gameOver(m_score);
        return;
    }
    // There's a little trick here:
    // searchAndErase() will cause m_animator to emit removeFinished()
    // If there wasn't m_placeBalls var
    // it would cause an infinite loop like this:
    // SaE()->removeAnimFinished()->next3Balls()->bornAnimFinished()->
    // SaE()->removeAnimFinished()->next3Balls()->...
    // etc etc
    m_placeBalls = false;
    // after placing new balls new 5-in-a-row chunks can occur
    // so we need to check for them
    //
    // And because of that we check for gameOver in removeAnimFinished()
    // rather than here - there's a chance that searchAndErase() will remove
    // balls making some free cells to play in
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
                    m_itemsToDelete.append(m_field[i][y]);
                    m_field[i][y] = 0;
                    m_numFreeCells++;
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
                    m_itemsToDelete.append(m_field[x][j]);
                    m_field[x][j] = 0;
                    m_numFreeCells++;
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
                    m_itemsToDelete.append(m_field[i][j]);
                    m_field[i][j] = 0;
                    m_numFreeCells++;
                }
            }
            else
                continue;
        }

    // up-right diagonal
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=4; y<FIELD_SIZE; ++y)
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
                    m_itemsToDelete.append(m_field[i][j]);
                    m_field[i][j] = 0;
                    m_numFreeCells++;
                }
            }
            else
                continue;
        }

    // after it finishes slot removeAnimFinished() will be called
    // if m_itemsToDelete is empty removeAnimFinished() will be called immediately
    m_animator->animateRemove( m_itemsToDelete );
}

void KLinesScene::moveFocusLeft()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }

    QPointF pos = m_focusItem->pos();
    pos.rx() -= m_cellSize;
    if( pos.x() < 0 )
        pos.setX( (FIELD_SIZE - 1)*m_cellSize );
    m_focusItem->setPos( pos );
}

void KLinesScene::moveFocusRight()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }

    QPointF pos = m_focusItem->pos();
    pos.rx() += m_cellSize;
    if( pos.x() > (FIELD_SIZE-1)*m_cellSize )
        pos.setX( 0 );
    m_focusItem->setPos( pos );
}

void KLinesScene::moveFocusUp()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }

    QPointF pos = m_focusItem->pos();
    pos.ry() -= m_cellSize;
    if( pos.y() < 0 )
        pos.setY( (FIELD_SIZE - 1)*m_cellSize );
    m_focusItem->setPos( pos );
}

void KLinesScene::moveFocusDown()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }

    QPointF pos = m_focusItem->pos();
    pos.ry() += m_cellSize;
    if( pos.y() > (FIELD_SIZE-1)*m_cellSize )
        pos.setY( 0 );
    m_focusItem->setPos( pos );
}

void KLinesScene::cellSelected()
{
    if( !m_focusItem->isVisible() )
        m_focusItem->show();

    // we're taking the center of the cell
    selectOrMove( pixToField( m_focusItem->pos() + QPointF(m_cellSize/2,m_cellSize/2) ) );
}

void KLinesScene::saveUndoInfo()
{
    // save field state to undoInfo
    for(int x=0;x<FIELD_SIZE;++x)
        for(int y=0; y<FIELD_SIZE;++y)
            // NumColors represents no color
            m_undoInfo.fcolors[x][y] = ( m_field[x][y] ? m_field[x][y]->color() : NumColors );
    m_undoInfo.numFreeCells = m_numFreeCells;
    m_undoInfo.score = m_score;
    m_undoInfo.nextColors = m_nextColors;

    emit enableUndo(true);
}

// Brings m_field and some other vars to the state it was before last turn
void KLinesScene::undo()
{
    if( m_selPos.isValid() )
        m_field[m_selPos.x][m_selPos.y]->stopAnimation();

    BallColor col;
    for(int x=0;x<FIELD_SIZE;++x)
        for(int y=0; y<FIELD_SIZE;++y)
        {
            col = m_undoInfo.fcolors[x][y];
            if(col == NumColors) // no ball
            {
                if( m_field[x][y] )
                {
                    removeItem( m_field[x][y] );
                    delete m_field[x][y];
                    m_field[x][y] = 0;
                }
                continue;
            }

            if( m_field[x][y] )
            {
                if( m_field[x][y]->color() != col )
                    m_field[x][y]->setColor(col);
                //else live it as it is
            }
            else
            {
                BallItem *item = new BallItem(this);
                item->setColor(col);
                item->setPos( fieldToPix( FieldPos(x,y) ) );
                item->show();
                m_field[x][y] = item;
            }
        }
    m_numFreeCells = m_undoInfo.numFreeCells;
    m_score = m_undoInfo.score;
    m_nextColors = m_undoInfo.nextColors;

    m_selPos = FieldPos();

    emit scoreChanged(m_score);
    emit nextColorsChanged();
    emit enableUndo(false);
}

void KLinesScene::drawBackground(QPainter *p, const QRectF&)
{
    // FIXME dimsuz: temp
    for(int x=0; x<m_cellSize*FIELD_SIZE;x+=m_cellSize)
        for(int y=0; y<m_cellSize*FIELD_SIZE;y+=m_cellSize)
            p->drawPixmap( x, y, KLinesRenderer::self()->backgroundTilePixmap() );
}

#include "scene.moc"