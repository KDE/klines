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
 * along with KLines; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#ifndef BALL_ITEM_H
#define BALL_ITEM_H

#include <QGraphicsPixmapItem>
#include <QTimeLine>

#include "commondefs.h"

/**
 *  QGraphicsItem for Ball
 */
class BallItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit BallItem( QGraphicsScene* parent );
    /**
     *  Sets ball's color
     *  @param setPix specifies whether to set corresponding ball pixmap to this
     *  item. In rare cases this may not be needed.
     *  (for example when the ball is created and born animation is played immediately)
     */
    void setColor( BallColor c, bool setPix = true );
    /**
     *  @return color of the ball
     */
    BallColor color() const { return m_color; }
    /**
     * Starts "Selected" animation
     */
    void startSelectedAnimation();
    /**
     *  Interrupts animation
     */
    void stopAnimation();

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 1 };
    virtual int type() const { return Type; }
private slots:
    void animFrameChanged(int);
private:
    /**
     *  Timeline for controlling animations
     */
    QTimeLine m_timeLine;
    /**
     *  Color of the ball
     */
    BallColor m_color;
};

#endif
