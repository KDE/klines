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
#ifndef KL_SCENE_H
#define KL_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <KRandomSequence>

#include "commondefs.h"

static const int FIELD_SIZE=9;

class KLinesRenderer;
class BallItem;

class KLinesScene : public QGraphicsScene
{
public:
    KLinesScene( QObject *parent );
    ~KLinesScene();

    void resizeScene( int width, int height );
    /**
     *  Brings in next three balls to scene
     */
    void nextThreeBalls();
private:
    struct FieldPos
    {
        int x;
        int y;
        FieldPos( int _x=-1, int _y=-1) : x(_x), y(_y) { }
        bool isValid() const { return (x != -1 && y != -1); }
    };

    /**
     *  Creates a ball of random color and places it in random free cell
     */
    void placeRandomBall();

    /**
     *  Field coords to pixel coords
     */
    inline QPointF fieldToPix(const FieldPos& fpos) const {
        return QPointF( fpos.x*32+2, fpos.y*32+2 );
    }
    inline FieldPos pixToField( const QPointF& p ) const { 
        return FieldPos(static_cast<int>(p.x()/32), static_cast<int>(p.y()/32)); }

    virtual void drawBackground( QPainter*, const QRectF& );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );

    /**
     *  This array represents the play field.
     *  Each cell holds the pointer to BallItem
     *  or 0 if there's no ball in that cell
     */
    BallItem* m_field[FIELD_SIZE][FIELD_SIZE];
    KLinesRenderer* m_renderer;

    KRandomSequence m_randomSeq;

    /**
     *  Position of selected ball (-1,-1) if none
     */
    FieldPos m_selPos;
    /**
     *  Number of balls currently in field
     */
    int m_numBalls;
};

class KLinesView : public QGraphicsView
{
public:
    KLinesView( KLinesScene* scene, QWidget *parent );
private:
    void resizeEvent(QResizeEvent *);
};

#endif
