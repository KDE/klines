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
    /**
     *  Creates a ball of random color and places it in random free cell
     */
    void placeRandomBall();

    /**
     *  Field coords to pixel coords
     */
    inline QPointF fieldToPix(int fieldX, int fieldY) const {
        return QPointF( fieldX*32+2, fieldY*32+2 );
    }
    inline int pixToFieldX( const QPointF& p ) const { return static_cast<int>(p.x()/32); }
    inline int pixToFieldY( const QPointF& p ) const { return static_cast<int>(p.y()/32); }
private:
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
     *  Currently selected ball (0 if none)
     */
    BallItem *m_selectedBall;
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
