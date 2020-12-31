/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BALL_ITEM_H
#define BALL_ITEM_H

#include <KGameRenderedItem>
#include <QTimeLine>

#include "commondefs.h"

/**
 *  KGameRenderedItem for Ball
 */
class BallItem : public QObject, public KGameRenderedItem
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
    int type() const override { return Type; }
private Q_SLOTS:
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
