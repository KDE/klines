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
#ifndef BALL_ITEM_H
#define BALL_ITEM_H

#include <QGraphicsPixmapItem>
#include <QTimeLine>

#include "commondefs.h"

class KLinesRenderer;

class BallItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    BallItem( QGraphicsScene* parent, const KLinesRenderer* r );

    void setColor( BallColor c );
    BallColor color() const { return m_color; }
    /**
     *  Starts animation.
     *  When animation will finish, animationFinished() signal 
     *  will be emitted (except for animation types which are looped)
     *
     *  @param type type of animation sequence to play
     */
    void startAnimation( BallAnimationType type );
    /**
     *  Interrupts current animation
     */
    void stopAnimation();
signals:
    /**
     *  Emitted when animation finishes.
     *  If current animation type is looped, this signal won't be emitted
     */
    // FIXME dimsuz: implement
    void animationFinished();
private slots:
    void animFrameChanged(int);
private:
    /**
     *  Renderer used to render ball's pixmaps
     */
    const KLinesRenderer* m_renderer;
    /**
     *  Timeline for controlling animations
     */
    QTimeLine m_timeLine;
    /**
     *  Color of the ball
     */
    BallColor m_color;
    /**
     *  Type of running animation
     * FIXME dimsuz: reset to something like NoAnimation when it isnt running
     */
    BallAnimationType m_curAnim;
};

#endif
