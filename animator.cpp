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


KLinesAnimator::KLinesAnimator( KLinesScene* scene )
    : m_scene(scene), m_movingBall(0)
{
    connect(&m_timeLine, SIGNAL(frameChanged(int)), SLOT(animFrameChanged(int)) );
    connect(&m_timeLine, SIGNAL(finished()), SIGNAL(moveFinished()));
}

void KLinesAnimator::animateMove( const FieldPos& from, const FieldPos& to )
{
    findPath(from, to);
    m_from = from;
    m_to = to;

    m_timeLine.setDuration(300);
    QPointF p1 = m_scene->fieldToPix(m_from);
    QPointF p2 = m_scene->fieldToPix(m_to);

    m_movingBall = m_scene->ballAt(m_from);

    // FIXME dimsuz: comment
    if(p2.x() - p1.x() != 0)
        m_timeLine.setFrameRange(static_cast<int>(p1.x()), static_cast<int>(p2.x()));
    else
        m_timeLine.setFrameRange(static_cast<int>(p1.y()), static_cast<int>(p2.y()));

    m_timeLine.start();
}

void KLinesAnimator::findPath( const FieldPos& from, const FieldPos& to )
{
    // Implementation of A* pathfinding algorithm
    // Thanks to Patrick Lester for excellent tutorial on gamedev.net.
    // See http://www.gamedev.net/reference/articles/article2003.asp
    
    QList<PathNode*> openList;
    QList<PathNode*> closedList;

    openList.append( new PathNode(from) );

    PathNode *curNode=0;
    bool pathFound = false;
    while(true)
    {
        // find the square with lowes F(=G+H) on the open list
        PathNode *minF = openList.at(0);
        for(int i=1; i<openList.count(); ++i)
        {
            if( openList.at(i)->F < minF->F )
                minF = openList.at(i);
        }
        kDebug() << "minF:" << minF->F << endl;
        // move it to closed list
        closedList.append(minF);
        openList.removeAll(minF);

        curNode = minF;

        kDebug() << "Current node (" << curNode->pos.x << "," << curNode->pos.y << ")" << endl;

        // for each of adjasent 4 squares (upper,lower, on the left and on the right)...
        QList<FieldPos> adjasentSquares;
        int x = curNode->pos.x;
        int y = curNode->pos.y;
        if( x != 0 ) adjasentSquares.append( FieldPos(x-1,y) );
        if( y != 0 ) adjasentSquares.append( FieldPos(x,y-1) );
        if( x != FIELD_SIZE-1 ) adjasentSquares.append( FieldPos(x+1,y) );
        if( y != FIELD_SIZE-1 ) adjasentSquares.append( FieldPos(x,y+1) );

        foreach( FieldPos pos, adjasentSquares )
        {

            if( m_scene->ballAt(pos) != 0 ) // skip non-walkable cells
            {
                kDebug() << "node (" << pos.x << "," << pos.y << ") contains ball - skipping" << endl;
                continue;
            }

            kDebug() << "looking at adjasent node (" << pos.x << "," << pos.y << ")" << endl;

            // skip if closed list contains this square
            bool found=false;
            for(int i=0;i<closedList.count(); ++i)
                if( closedList.at(i)->pos == pos )
                {
                    found = true;
                    break;
                }
            if(found)
            {
                kDebug() << "node (" << pos.x << "," << pos.y << ") is in closed list - skipping" << endl;
                continue;
            }

            // search for node with position 'pos' in openList
            PathNode *node = 0;
            for(int i=0;i<openList.count(); ++i)
                if( openList.at(i)->pos == pos )
                {
                    node = openList.at(i);
                    break;
                }

            if(!node) // not found
            {
                kDebug() << "it is not in open list. adding" << endl;
                node = new PathNode( pos );
                node->parent = curNode;
                node->G = curNode->G + 10;
                // h is manhattanLength from node to target square
                node->H = sqrt( pow( (to.x - pos.x)*10, 2 ) + pow( (to.y - pos.y)*10, 2 ) );
                node->F = node->G+node->H;
                openList.append( node );

                kDebug() << "G:" << node->G << " H:" << node->H << endl;
            }
            else
            {
                // check if this path to square is better
                kDebug() << "it is in open list. cheking if this path is better..." << endl;
                if( curNode->G + 10 < node->G )
                {
                    // yup, it's better, reparent and recalculate G,F
                    node->parent = curNode;
                    node->G = curNode->G + 10;
                    node->F = node->G + node->H;
                }
            }
            kDebug() << "====" << endl;
        } // foreach

        // exit conditions:
        // a) if closeList contains "to"
        // b) we can't find "to" in closedList and openlist is empty => no path exists
        bool found=false;
        for(int i=0;i<closedList.count(); ++i)
            if( closedList.at(i)->pos == to )
            {
                found = true;
                // let's save last node in curNode variable
                curNode = closedList.at(i);
                break;
            }
        if(found)
        {
            pathFound = true;
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
        kDebug() << "====" << endl;
        kDebug() << "and the path is:" <<endl;
        // restoring path starting from last node:
        PathNode* node = curNode;
        while(node)
        {
            kDebug() << "(" << node->pos.x << "," << node->pos.y << ")" << endl;
            node = node->parent;
        }
    }
    else
        kDebug() << "no path found!" << endl;
}

void KLinesAnimator::animFrameChanged(int frame)
{
    QPointF p1 = m_scene->fieldToPix(m_from);
    QPointF p2 = m_scene->fieldToPix(m_to);
    qreal x=0, y=0;
    // FIXME dimsuz: comment
    if(p2.x()-p1.x() != 0)
    {
        x = frame;
        y = (frame-p1.x()) * (p2.y()-p1.y()) / (p2.x()-p1.x()) + p1.y();
    }
    else
    {
        x = p2.x();
        y = frame;
    }
    m_movingBall->setPos(x,y);
}

#include "animator.moc"
