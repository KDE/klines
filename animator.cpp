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
#include "animator.h"
#include "scene.h"
#include "ballitem.h"
#include "renderer.h"

#include <kdebug.h>
#include <math.h> // for pow, sqrt

// Needed by A* pathfinding algorithm
struct PathNode
{
    FieldPos pos;
    PathNode *parent;
    int G;
    float H;
    float F;
    PathNode( const FieldPos& fpos, PathNode* p = 0, int g=0, int h=0 )
        : pos(fpos), parent(p), G(g), H(h), F(g+h) { }
};

// helper function - used in findPath()
// returns:
//         -1 - if position not found
//         index of node in list if position is found
static inline int indexOfNodeWithPos( const FieldPos& pos, const QList<PathNode*>& list )
{
    for(int i=0;i<list.count(); ++i)
        if( list.at(i)->pos == pos )
            return i;

    return -1;
}

KLinesAnimator::KLinesAnimator( KLinesScene* scene )
    : m_scene(scene), m_movingBall(0)
{
    connect(&m_moveTimeLine, SIGNAL(frameChanged(int)), SLOT(moveAnimationFrame(int)) );
    connect(&m_moveTimeLine, SIGNAL(finished()), SIGNAL(moveFinished()));

    m_removeTimeLine.setDuration(300);
    m_removeTimeLine.setCurveShape(QTimeLine::LinearCurve);
    // we setup here one 'empty' frame at the end, because without it
    // m_scene will delete 'burned' items in removeAnimFinished() slot so quickly
    // that last frame won't get shown in the scene
    m_removeTimeLine.setFrameRange(0, KLinesRenderer::self()->numDieFrames());

    connect(&m_removeTimeLine, SIGNAL(frameChanged(int)), SLOT(removeAnimationFrame(int)) );
    connect(&m_removeTimeLine, SIGNAL(finished()), SIGNAL(removeFinished()));

    m_bornTimeLine.setDuration(400);
    m_bornTimeLine.setCurveShape(QTimeLine::LinearCurve);
    m_bornTimeLine.setFrameRange(0, KLinesRenderer::self()->numBornFrames()-1);

    connect(&m_bornTimeLine, SIGNAL(frameChanged(int)), SLOT(bornAnimationFrame(int)) );
    connect(&m_bornTimeLine, SIGNAL(finished()), SIGNAL(bornFinished()));
}

bool KLinesAnimator::isAnimating() const
{
    return (m_bornTimeLine.state() == QTimeLine::Running
            || m_moveTimeLine.state() == QTimeLine::Running
            || m_removeTimeLine.state() == QTimeLine::Running);
}

void KLinesAnimator::animateMove( const FieldPos& from, const FieldPos& to )
{
    findPath(from, to);

    if(m_foundPath.isEmpty())
        return;

    m_movingBall = m_scene->ballAt(from);
    m_movingBall->stopAnimation();

    int numPoints = m_foundPath.count();
    // there will be numPoints-1 intervals of
    // movement (interval=cell). We want each of them to take 100ms
    m_moveTimeLine.setDuration((numPoints-1)*100);
    // each interval will take cellSize frames
    m_moveTimeLine.setFrameRange(0, (numPoints-1)*KLinesRenderer::self()->cellSize());
    m_moveTimeLine.setCurrentTime(0);
    m_moveTimeLine.start();
}

void KLinesAnimator::animateRemove( const QList<BallItem*>& list )
{
    m_moveTimeLine.stop();
    m_removeTimeLine.stop();

    if(list.isEmpty())
    {
        emit removeFinished();
        return;
    }

    m_removedBalls = list;
    m_removeTimeLine.start();
}

void KLinesAnimator::animateBorn( const QList<BallItem*>& list )
{
    m_bornBalls = list;
    m_bornTimeLine.start();
}

void KLinesAnimator::moveAnimationFrame(int frame)
{
    int cellSize = KLinesRenderer::self()->cellSize();
    int intervalNum = frame/cellSize;

    if(intervalNum == m_foundPath.count()-1)
    {
        m_movingBall->setPos(m_scene->fieldToPix(m_foundPath.last()));
        return;
    }
    // determine direction of movement on this interval
    int kx=0, ky=0;

    FieldPos from = m_foundPath.at(intervalNum);
    FieldPos to = m_foundPath.at(intervalNum+1);

    if( to.x - from.x > 0 )
        kx = 1;
    else if( to.x - from.x < 0 )
        kx = -1;
    else
        kx = 0;

    if( to.y - from.y > 0 )
        ky = 1;
    else if( to.y - from.y < 0 )
        ky = -1;
    else
        ky = 0;

    int frameWithinInterval = frame%cellSize;
    QPointF pos = m_scene->fieldToPix(from);
    m_movingBall->setPos( pos.x()+kx*frameWithinInterval,
                          pos.y()+ky*frameWithinInterval );
}

void KLinesAnimator::removeAnimationFrame(int frame)
{
    if(frame == KLinesRenderer::self()->numDieFrames())
        return;
    foreach(BallItem* ball, m_removedBalls)
        ball->setPixmap( KLinesRenderer::self()->diePixmap(ball->color(), frame) );
}

void KLinesAnimator::bornAnimationFrame(int frame)
{
    foreach(BallItem* ball, m_bornBalls)
        ball->setPixmap( KLinesRenderer::self()->bornPixmap(ball->color(), frame) );
}

void KLinesAnimator::findPath( const FieldPos& from, const FieldPos& to )
{
    // Implementation of A* pathfinding algorithm
    // Thanks to Patrick Lester for excellent tutorial on gamedev.net.
    // See http://www.gamedev.net/reference/articles/article2003.asp

    QList<PathNode*> openList;
    QList<PathNode*> closedList;

    m_foundPath.clear();

    openList.append( new PathNode(from) );

    PathNode *curNode=0;
    bool pathFound = false;
    // see exit conditions at the end of while loop below
    while(true)
    {
        // find the square with lowes F(=G+H) on the open list
        PathNode *minF = openList.at(0);
        for(int i=1; i<openList.count(); ++i)
            if( openList.at(i)->F < minF->F )
                minF = openList.at(i);

        // move it to closed list
        closedList.append(minF);
        openList.removeAll(minF);

        curNode = minF;

        // for each of adjasent 4 squares (upper,lower, on the left and on the right)...
        QList<FieldPos> adjasentSquares;
        int x = curNode->pos.x;
        int y = curNode->pos.y;
        if( x != 0 ) adjasentSquares.append( FieldPos(x-1,y) );
        if( y != 0 ) adjasentSquares.append( FieldPos(x,y-1) );
        if( x != FIELD_SIZE-1 ) adjasentSquares.append( FieldPos(x+1,y) );
        if( y != FIELD_SIZE-1 ) adjasentSquares.append( FieldPos(x,y+1) );

        foreach( const FieldPos &pos, adjasentSquares )
        {
            if( m_scene->ballAt(pos) != 0 ) // skip non-walkable cells
                continue;

            // skip if closed list contains this square
            if(indexOfNodeWithPos(pos, closedList) != -1)
                continue;

            // search for node with position 'pos' in openList
            int idx = indexOfNodeWithPos(pos, openList);
            if(idx == -1) // not found
            {
                PathNode *node = new PathNode( pos );
                node->parent = curNode;
                node->G = curNode->G + 10;
                // h is manhattanLength from node to target square
                node->H = sqrt( pow( (to.x - pos.x)*10, 2. ) + pow( (to.y - pos.y)*10, 2. ) );
                node->F = node->G+node->H;
                openList.append( node );
            }
            else
            {
                PathNode *node = openList.at(idx);
                // check if this path to square is better
                if( curNode->G + 10 < node->G )
                {
                    // yup, it's better, reparent and recalculate G,F
                    node->parent = curNode;
                    node->G = curNode->G + 10;
                    node->F = node->G + node->H;
                }
            }
        } // foreach

        // exit conditions:
        // a) if closeList contains "to"
        // b) we can't find "to" in closedList and openlist is empty
        //    => no path exists
        int idx = indexOfNodeWithPos(to, closedList);
        if(idx != -1)
        {
            pathFound = true;
            // let's save last node in curNode variable
            curNode = closedList.at(idx);
            break; // while
        }
        else if(openList.isEmpty())
        {
            pathFound = false;
            break;
        }
    }

    if(pathFound)
    {
        // restoring path starting from last node:
        PathNode* node = curNode;
        while(node)
        {
            m_foundPath.prepend( node->pos );
            node = node->parent;
        }
    }

    // cleanup
    qDeleteAll( openList );
    qDeleteAll( closedList );
}

#include "animator.moc"
