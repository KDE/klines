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
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QTimeLine>
#include "commondefs.h"

class KLinesScene;
class BallItem;

class KLinesAnimator : public QObject
{
    Q_OBJECT
public:
    KLinesAnimator( KLinesScene *scene );
    void animateMove( const FieldPos& from, const FieldPos& to );
    void animateRemove( const QList<BallItem*>& list );
    void animateBorn( const QList<BallItem*>& list );
    bool isAnimating() const;
signals:
    void moveFinished();
    void removeFinished();
    void bornFinished();
private slots:
    void moveAnimationFrame(int);
    void removeAnimationFrame(int);
    void bornAnimationFrame(int);
private:
    /**
     *  Implements A* pathfinding algorithm.
     */
    void findPath(const FieldPos& from, const FieldPos& to);

    QTimeLine m_moveTimeLine;
    QTimeLine m_removeTimeLine;
    QTimeLine m_bornTimeLine;

    KLinesScene* m_scene;
    BallItem* m_movingBall;
    QList<FieldPos> m_foundPath;
    QList<BallItem*> m_removedBalls;
    QList<BallItem*> m_bornBalls;
};

#endif
