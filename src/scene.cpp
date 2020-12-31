/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006-2008 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "scene.h"
#include "ballitem.h"
#include "previewitem.h"
#include "animator.h"
#include "renderer.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QSet>
#include "klines_debug.h"

#include <KGamePopupItem>
#include <KLocalizedString>

inline uint qHash( FieldPos pos )
{
    return qHash( QPair<int,int>(pos.x,pos.y) );
}

KLinesScene::KLinesScene( QObject* parent )
    : QGraphicsScene(parent),
      m_randomSeq(QRandomGenerator::global()->generate()),
      m_playFieldBorderSize(0), m_numFreeCells(FIELD_SIZE*FIELD_SIZE),
      m_score(0), m_bonusScore(0), m_cellSize(32), m_previewZoneVisible(true)
{
    m_animator = new KLinesAnimator(this);
    connect(m_animator, &KLinesAnimator::moveFinished, this, &KLinesScene::moveAnimFinished);
    connect(m_animator, &KLinesAnimator::removeFinished, this, &KLinesScene::removeAnimFinished);
    connect(m_animator, &KLinesAnimator::bornFinished, this, &KLinesScene::bornAnimFinished);

    m_focusItem = new QGraphicsRectItem( QRectF(0, 0, m_cellSize, m_cellSize), nullptr);
    m_focusItem->setZValue(1.0);
    m_focusItem->setPen( Qt::DashLine );
    addItem(m_focusItem);

    m_previewItem = new PreviewItem(this);
    m_previewItem->setPos( 0, 0 );
    addItem(m_previewItem);

    m_popupItem = new KGamePopupItem;
    addItem(m_popupItem);

    startNewGame();
}

void KLinesScene::startNewGame()
{
    if(m_animator->isAnimating())
        return;

    // reset all vars
    m_selPos = FieldPos();
    m_numFreeCells = FIELD_SIZE*FIELD_SIZE;
    m_score = 0;
    m_bonusScore = 0;
    m_placeBalls = true;
    m_gameOver = false;
    m_itemsToDelete.clear();
    m_nextColors.clear();
    m_focusItem->setPos(0, 0);
    m_focusItem->hide();

    m_popupItem->forceHide();

    // remove all ball items from the scene leaving other items untouched
    const QList<QGraphicsItem*> itemlist = items();
    for (QGraphicsItem* item : itemlist) {
        BallItem* ball = qgraphicsitem_cast<BallItem*>(item);
        if( ball )
        {
            removeItem(item);
            delete item;
        }
    }

    for(int x=0; x<FIELD_SIZE; ++x)
        for(int y=0; y<FIELD_SIZE; ++y)
            m_field[x][y] = nullptr;

    // init m_nextColors
    for(int i=0; i<3; i++)
    {
        // random color
        BallColor c = static_cast<BallColor>(m_randomSeq.bounded(static_cast<int>(NumColors)));
        m_nextColors.append(c);
    }

    Q_EMIT stateChanged(QStringLiteral( "not_undoable" ));

    nextThreeBalls();
}

KLinesScene::~KLinesScene()
{
    delete m_animator;
}


void KLinesScene::endGame()
{
    gameOverHandler();
}

void KLinesScene::resizeScene(int width,int height)
{
    // store focus item field pos (calculated using old cellSize)
    FieldPos focusRectFieldPos = pixToField( m_focusItem->pos() );

    bool hasBorder = KLinesRenderer::hasBorderElement();

    int minDim = qMin( width, height );
    // border width is hardcoded to be half of cell size.
    // take it into account if it exists
    m_cellSize = hasBorder ? minDim/(FIELD_SIZE+1) : minDim/FIELD_SIZE;

    // set it only if current theme supports it
    m_playFieldBorderSize = hasBorder ? m_cellSize/2 : 0;

    int boardSize = m_cellSize * FIELD_SIZE;
    if ( m_previewZoneVisible && boardSize +m_playFieldBorderSize*2 + m_cellSize > width) // No space enough for balls preview
    {
        minDim = width;
        m_cellSize = hasBorder ? (minDim - m_cellSize - m_playFieldBorderSize*2)/FIELD_SIZE : (minDim - m_cellSize)/FIELD_SIZE;
        boardSize = m_cellSize * FIELD_SIZE;
    }


    m_playFieldRect.setX( (width - (m_previewZoneVisible ? m_cellSize : 0))/2 - boardSize/2 - m_playFieldBorderSize );
    m_playFieldRect.setY( height/2 - boardSize/2 - m_playFieldBorderSize );

    m_playFieldRect.setWidth( boardSize + m_playFieldBorderSize*2 );
    m_playFieldRect.setHeight( boardSize + m_playFieldBorderSize*2 );

    setSceneRect( 0, 0, width, height );

    // sets render sizes for cells
    KLinesRenderer::setCellSize( m_cellSize );
    QSize cellSize(m_cellSize, m_cellSize);

    // re-render && recalc positions for all balls
    for( int x=0; x<FIELD_SIZE; ++x)
        for(int y=0; y< FIELD_SIZE; ++y)
        {
            if( m_field[x][y] )
            {
                // this will update pixmap
                m_field[x][y]->setRenderSize(cellSize);
                m_field[x][y]->setPos( fieldToPix( FieldPos(x,y) ) );
                m_field[x][y]->setColor( m_field[x][y]->color() );
            }
        }

    m_focusItem->setRect( QRect(0,0, m_cellSize, m_cellSize) );
    m_focusItem->setPos( fieldToPix( focusRectFieldPos ) );

    int previewOriginY = height / 2 - (3 * m_cellSize) / 2;
    int previewOriginX = m_playFieldRect.x() + m_playFieldRect.width();
    m_previewItem->setPos( previewOriginX, previewOriginY );
    m_previewItem->setPreviewColors( m_nextColors );

    //qCDebug(KLINES_LOG) << "resize:" << width << "," << height << "; cellSize:" << m_cellSize;
}

void KLinesScene::endTurn()
{
    if( m_gameOver )
        return;

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
        BallColor c = static_cast<BallColor>(m_randomSeq.bounded(static_cast<int>(NumColors)));
        m_nextColors[i] = c;
    }

    m_previewItem->setPreviewColors( m_nextColors );

    m_animator->animateBorn( newItems );
}

void KLinesScene::setPreviewZoneVisible( bool visible )
{
    if (visible == m_previewZoneVisible)
        return;

    m_previewZoneVisible = visible;
    m_previewItem->setVisible( visible );
    resizeScene((int) width(), (int) height());
    invalidate( sceneRect() );
}

BallItem* KLinesScene::randomlyPlaceBall(BallColor c)
{
    m_numFreeCells--;
    if(m_numFreeCells < 0)
    {
        // restore m_numFreeCells value, it will trigger
        // saveAndErase() after bornAnimFinished to check if
        // we have 5-in-a-row to erase
        m_numFreeCells = 0;
        return nullptr; // game over, we won't create more balls
    }

    int posx = -1, posy = -1;
    // let's find random free cell
    do
    {
        posx = m_randomSeq.bounded(FIELD_SIZE);
        posy = m_randomSeq.bounded(FIELD_SIZE);
    } while( m_field[posx][posy] != nullptr );

    BallItem* newBall = new BallItem( this);
    newBall->setColor(c, false); // pixmap will be set by born animation
    newBall->setPos( fieldToPix( FieldPos(posx,posy) ) );

    m_field[posx][posy] = newBall;
    return newBall;
}

void KLinesScene::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    QGraphicsScene::mousePressEvent(ev);
    QRect boardRect = m_playFieldRect.adjusted( m_playFieldBorderSize,
                                                m_playFieldBorderSize,
                                                -m_playFieldBorderSize,
                                                -m_playFieldBorderSize );

    if ( !boardRect.contains( ev->scenePos().toPoint() ) )
        return;

    selectOrMove( pixToField(ev->scenePos()) );
}

void KLinesScene::selectOrMove( FieldPos fpos )
{
    if (fpos.x < 0 || fpos.y < 0) {
        return;
    }
    if( m_animator->isAnimating() )
        return;
    if( m_field[fpos.x][fpos.y] ) // ball was selected
    {
        if( m_selPos.isValid() )
        {
            m_field[m_selPos.x][m_selPos.y]->stopAnimation();

            if ( m_selPos == fpos )
            {
                m_selPos.x = m_selPos.y = -1; // invalidate position
                return;
            }
        }

        m_field[fpos.x][fpos.y]->startSelectedAnimation();
        m_selPos = fpos;
    }
    else // move selected ball to new location
    {
        if( m_selPos.isValid() && m_field[fpos.x][fpos.y] == nullptr )
        {
            saveUndoInfo();
            // start move animation
            // slot moveAnimFinished() will be called when it finishes
            bool pathExists = m_animator->animateMove(m_selPos, fpos);
            if(!pathExists)
            {
                m_popupItem->setMessageTimeout(2500);
                m_popupItem->showMessage(i18n("There is no path from the selected piece to this cell"), KGamePopupItem::BottomLeft);
            }
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

    m_field[m_selPos.x][m_selPos.y] = nullptr; // no more ball here
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
        // game over
        gameOverHandler();
        return;
    }

    if( m_itemsToDelete.isEmpty() && m_placeBalls)
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

        for (BallItem* item : qAsConst(m_itemsToDelete)) {
            removeItem(item);
            delete item;
        }
        m_itemsToDelete.clear();

        if(numBallsErased)
            Q_EMIT scoreChanged(m_score);
    }
}

void KLinesScene::bornAnimFinished()
{
    // note that if m_numFreeCells == 0, we still need to
    // check for possible 5-in-a-row balls, i.e. call searchAndErase()
    // So there's another gameOver-check in removeAnimFinished()
    if( m_numFreeCells < 0 )
    {
        gameOverHandler();
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

    // QSet - to exclude adding duplicates
    QSet<FieldPos> positionsToDelete;

    // horizontal chunks searching
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=0;y<FIELD_SIZE; ++y)
        {
            if(m_field[x][y] == nullptr)
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
                    positionsToDelete.insert( FieldPos(i,y) );
                }
            }
            else
                continue;
        }

    // vertical chunks searching
    for(int y=0; y<FIELD_SIZE-4; ++y)
        for(int x=0;x<FIELD_SIZE; ++x)
        {
            if(m_field[x][y] == nullptr)
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
                    positionsToDelete.insert( FieldPos(x,j) );
                }
            }
            else
                continue;
        }

    // down-right diagonal
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=0;y<FIELD_SIZE-4; ++y)
        {
            if(m_field[x][y] == nullptr)
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
                    positionsToDelete.insert( FieldPos(i,j) );
                }
            }
            else
                continue;
        }

    // up-right diagonal
    for(int x=0; x<FIELD_SIZE-4; ++x)
        for(int y=4; y<FIELD_SIZE; ++y)
        {
            if(m_field[x][y] == nullptr)
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
                    positionsToDelete.insert( FieldPos(i,j) );
                }
            }
            else
                continue;
        }

    for (const FieldPos& pos : qAsConst(positionsToDelete)) {
        m_itemsToDelete.append(m_field[pos.x][pos.y]);
        m_field[pos.x][pos.y] = nullptr;
        m_numFreeCells++;
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
    FieldPos focusPos = pixToField( m_focusItem->pos() );
    focusPos.x--;
    if (focusPos.x < 0) // rotate on the torus
        focusPos.x = FIELD_SIZE - 1;

    m_focusItem->setPos ( fieldToPix( focusPos ) );
}

void KLinesScene::moveFocusRight()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }
    FieldPos focusPos = pixToField( m_focusItem->pos() );
    focusPos.x++;
    if (focusPos.x >= FIELD_SIZE) // rotate on the torus
        focusPos.x = 0;

    m_focusItem->setPos ( fieldToPix( focusPos ) );
}

void KLinesScene::moveFocusUp()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }
    FieldPos focusPos = pixToField( m_focusItem->pos() );
    focusPos.y--;
    if (focusPos.y < 0) // rotate on the torus
        focusPos.y = FIELD_SIZE - 1;

    m_focusItem->setPos ( fieldToPix( focusPos ) );
}

void KLinesScene::moveFocusDown()
{
    if( !m_focusItem->isVisible() )
    {
        m_focusItem->show();
        // no action for the first time
        return;
    }

    FieldPos focusPos = pixToField( m_focusItem->pos() );
    focusPos.y++;
    if (focusPos.y >= FIELD_SIZE) // rotate on the torus
        focusPos.y = 0;

    m_focusItem->setPos ( fieldToPix( focusPos ) );
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

    Q_EMIT stateChanged(QStringLiteral( "undoable" ));
}

// Brings m_field and some other vars to the state it was before last turn
void KLinesScene::undo()
{
    // do not allow undo during animation
    if(m_animator->isAnimating())
        return;

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
                    m_field[x][y] = nullptr;
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
                item->setRenderSize(KLinesRenderer::cellExtent());
                m_field[x][y] = item;
            }
        }
    m_numFreeCells = m_undoInfo.numFreeCells;
    m_score = m_undoInfo.score;
    m_nextColors = m_undoInfo.nextColors;

    m_selPos = FieldPos();

    m_previewItem->setPreviewColors( m_nextColors );

    Q_EMIT scoreChanged(m_score);

    Q_EMIT stateChanged(QStringLiteral( "not_undoable" ));
}

void KLinesScene::drawBackground(QPainter *p, const QRectF&)
{
    QPixmap tile = KLinesRenderer::backgroundTilePixmap();
    p->drawPixmap( 0, 0, KLinesRenderer::backgroundPixmap(sceneRect().size().toSize()) );
    p->drawPixmap( m_playFieldRect.x(), m_playFieldRect.y(),
                   KLinesRenderer::backgroundBorderPixmap( m_playFieldRect.size() ) );

    int startX = m_playFieldRect.x()+m_playFieldBorderSize;
    int maxX = m_playFieldRect.x()+m_cellSize*FIELD_SIZE;
    int startY = m_playFieldRect.y()+m_playFieldBorderSize;
    int maxY = m_playFieldRect.y()+m_cellSize*FIELD_SIZE;

    for(int x=startX; x<maxX; x+=m_cellSize)
        for(int y=startY; y<maxY; y+=m_cellSize)
            p->drawPixmap( x, y, tile );
}

void KLinesScene::gameOverHandler()
{
    if( m_gameOver )
        return; // don't emit twice
    m_gameOver = true;
    qCDebug(KLINES_LOG) << "GAME OVER";
    Q_EMIT stateChanged(QStringLiteral( "not_undoable" ));
    //Q_EMIT enableUndo(false);
    Q_EMIT gameOver(m_score);

    // disable auto-hide
    m_popupItem->setMessageTimeout(0);
    m_popupItem->showMessage(i18n("<h1>Game over</h1>"), KGamePopupItem::Center);
}


