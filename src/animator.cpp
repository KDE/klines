/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "animator.h"
#include "scene.h"
#include "ballitem.h"
#include "renderer.h"

#include <math.h> // for pow, sqrt

// Needed by A* pathfinding algorithm
struct PathNode
{
    FieldPos pos;
    PathNode *parent;
    int G;
    float H;
    float F;
    PathNode( FieldPos fpos, PathNode* p = nullptr, int g=0, int h=0 )
        : pos(fpos), parent(p), G(g), H(h), F(g+h) { }
};

// helper function - used in findPath()
// returns:
//         -1 - if position not found
//         index of node in list if position is found
static inline int indexOfNodeWithPos( FieldPos pos, const QList<PathNode*>& list )
{
    for(int i=0;i<list.count(); ++i)
        if( list.at(i)->pos == pos )
            return i;

    return -1;
}

KLinesAnimator::KLinesAnimator( KLinesScene* scene )
    : m_scene(scene), m_movingBall(nullptr)
{
    // timing & framing setup is done in corresponding animate* functions

    connect(&m_moveTimeLine, &QTimeLine::frameChanged, this, &KLinesAnimator::moveAnimationFrame);
    connect(&m_moveTimeLine, &QTimeLine::finished, this, &KLinesAnimator::moveFinished);

    m_removeTimeLine. setEasingCurve(QEasingCurve::Linear);
    connect(&m_removeTimeLine, &QTimeLine::frameChanged, this, &KLinesAnimator::removeAnimationFrame);
    connect(&m_removeTimeLine, &QTimeLine::finished, this, &KLinesAnimator::removeFinished);

    m_bornTimeLine. setEasingCurve(QEasingCurve::Linear);
    connect(&m_bornTimeLine, &QTimeLine::frameChanged, this, &KLinesAnimator::bornAnimationFrame);
    connect(&m_bornTimeLine, &QTimeLine::finished, this, &KLinesAnimator::slotBornFinished);
}

bool KLinesAnimator::isAnimating() const
{
    return (m_bornTimeLine.state() == QTimeLine::Running
            || m_moveTimeLine.state() == QTimeLine::Running
            || m_removeTimeLine.state() == QTimeLine::Running);
}

bool KLinesAnimator::animateMove( FieldPos from, FieldPos to )
{
    findPath(from, to);

    if(m_foundPath.isEmpty())
        return false;

    m_movingBall = m_scene->ballAt(from);
    m_movingBall->stopAnimation();

    int numPoints = m_foundPath.count();
    // there will be numPoints-1 intervals of
    // movement (interval=cell). We want each of them to take animDuration(MoveAnim) ms
    m_moveTimeLine.setDuration((numPoints-1)*KLinesRenderer::animDuration(KLinesRenderer::MoveAnim));
    // each interval will take cellSize frames
    m_moveTimeLine.setFrameRange(0, (numPoints-1)*KLinesRenderer::cellSize());
    m_moveTimeLine.setCurrentTime(0);
    m_moveTimeLine.start();
    return true;
}

void KLinesAnimator::animateRemove( const QList<BallItem*>& list )
{
    m_moveTimeLine.stop();
    m_removeTimeLine.stop();

    if(list.isEmpty())
    {
        Q_EMIT removeFinished();
        return;
    }

    m_removedBalls = list;

    // called here (not in constructor), to stay in sync in case theme's reloaded
    m_removeTimeLine.setDuration(KLinesRenderer::animDuration(KLinesRenderer::DieAnim));
    // we setup here one 'empty' frame at the end, because without it
    // m_scene will delete 'burned' items in removeAnimFinished() slot so quickly
    // that last frame won't get shown in the scene
    m_removeTimeLine.setFrameRange(0, KLinesRenderer::frameCount(KLinesRenderer::DieAnim));

    m_removeTimeLine.start();
}

void KLinesAnimator::animateBorn( const QList<BallItem*>& list )
{
    m_bornBalls = list;
    for (BallItem* ball : qAsConst(m_bornBalls)) {
        ball->setRenderSize(KLinesRenderer::cellExtent());
    }

    // called here (not in constructor), to stay in sync in case theme's reloaded
    m_bornTimeLine.setDuration(KLinesRenderer::animDuration(KLinesRenderer::BornAnim));
    m_bornTimeLine.setFrameRange(0, KLinesRenderer::frameCount(KLinesRenderer::BornAnim)-1);

    m_bornTimeLine.setCurrentTime( 0 );
    m_bornTimeLine.start();
}

void KLinesAnimator::moveAnimationFrame(int frame)
{
    int cellSize = m_moveTimeLine.endFrame() / (m_foundPath.count()-1);
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
    if(frame == KLinesRenderer::frameCount(KLinesRenderer::DieAnim))
        return;
    for (BallItem* ball : qAsConst(m_removedBalls)) {
	ball->setSpriteKey(KLinesRenderer::animationFrameId( KLinesRenderer::DieAnim,
                                                                 ball->color(), frame) );
    }
}

void KLinesAnimator::bornAnimationFrame(int frame)
{
    for (BallItem* ball : qAsConst(m_bornBalls)) {
        ball->setSpriteKey( KLinesRenderer::animationFrameId( KLinesRenderer::BornAnim,
                                                                 ball->color(), frame) );
    }
}

void KLinesAnimator::findPath( FieldPos from, FieldPos to )
{
    // Implementation of A* pathfinding algorithm
    // Thanks to Patrick Lester for excellent tutorial on gamedev.net.
    // See https://www.gamedev.net/articles/programming/artificial-intelligence/a-pathfinding-for-beginners-r2003/

    QList<PathNode*> openList;
    QList<PathNode*> closedList;

    m_foundPath.clear();

    openList.append( new PathNode(from) );

    PathNode *curNode=nullptr;
    bool pathFound = false;
    // see exit conditions at the end of while loop below
    while(true)
    {
        // find the square with lowest F(=G+H) on the open list
        PathNode *minF = openList.at(0);
        for(int i=1; i<openList.count(); ++i)
            if( openList.at(i)->F < minF->F )
                minF = openList.at(i);

        // move it to closed list
        closedList.append(minF);
        openList.removeAll(minF);

        curNode = minF;

        // for each of adjacent 4 squares (upper,lower, on the left and on the right)...
        QList<FieldPos> adjacentSquares;
        int x = curNode->pos.x;
        int y = curNode->pos.y;
        if( x != 0 ) adjacentSquares.append( FieldPos(x-1,y) );
        if( y != 0 ) adjacentSquares.append( FieldPos(x,y-1) );
        if( x != FIELD_SIZE-1 ) adjacentSquares.append( FieldPos(x+1,y) );
        if( y != FIELD_SIZE-1 ) adjacentSquares.append( FieldPos(x,y+1) );

        for (const FieldPos &pos : qAsConst(adjacentSquares)) {
            if( m_scene->ballAt(pos) != nullptr ) // skip non-walkable cells
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
        } // for

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

void KLinesAnimator::startGameOverAnimation()
{
    blockSignals(true);
    QList<BallItem*> balls;
    const QList<QGraphicsItem*> items = m_scene->items();
    BallItem *ball=nullptr;
    for (QGraphicsItem* item : items) {
        ball = qgraphicsitem_cast<BallItem*>(item);
        if(ball)
            balls.append(ball);
    }
    animateRemove(balls);
}

void KLinesAnimator::stopGameOverAnimation()
{
    blockSignals(false);
}

void KLinesAnimator::slotBornFinished()
{
    for (BallItem* ball : qAsConst(m_bornBalls)) {
	ball->setColor(ball->color(), true);
    }
    Q_EMIT bornFinished();
}


