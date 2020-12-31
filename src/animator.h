/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QTimeLine>
#include "commondefs.h"

class KLinesScene;
class BallItem;

/**
 *  Drives KLines animations
 */
class KLinesAnimator : public QObject
{
    Q_OBJECT
public:
    explicit KLinesAnimator( KLinesScene *scene );
    /**
     *  Starts animation of ball movement.
     *  When animation finishes moveFinished() signal is emitted
     *  @param from starting field position
     *  @param to   target field position
     *
     *  @return true is there exists a path (from,to), false otherwise
     */
    bool animateMove(FieldPos from, FieldPos to );
    /**
     *  Starts animation of ball deletion from field.
     *  Note that it doesn't do actual deletion - it just runs
     *  animation of deletion.
     *  When animation finishes removeFinished() signal is emitted
     *  @param list list of balls to 'remove'
     */
    void animateRemove( const QList<BallItem*>& list );
    /**
     *  Starts animation of ball movement.
     *  When animation finishes bornFinished() signal is emitted
     *  @param list list of balls to be 'born'
     */
    void animateBorn( const QList<BallItem*>& list );
    /**
     *  @return whether some animation is in progress
     */
    bool isAnimating() const;
    /**
     * Starts game over animation on the scene, shows game over message
     * TODO: does nothing useful yet
     */
    void startGameOverAnimation();
    /**
     * Stops game over animation
     * TODO: does nothing useful yet
     */
    void stopGameOverAnimation();
Q_SIGNALS:
    void moveFinished();
    void removeFinished();
    void bornFinished();
private Q_SLOTS:
    void moveAnimationFrame(int);
    void removeAnimationFrame(int);
    void bornAnimationFrame(int);

    void slotBornFinished();
private:
    /**
     *  Implements A* pathfinding algorithm.
     */
    void findPath(FieldPos from, FieldPos to);
    /**
     *  Timeline used to control movement animation
     */
    QTimeLine m_moveTimeLine;
    /**
     *  Timeline used to control deletion animation
     */
    QTimeLine m_removeTimeLine;
    /**
     *  Timeline used to control birth animation
     */
    QTimeLine m_bornTimeLine;
    /**
     *  Scene on which animations are played
     */
    KLinesScene* m_scene;
    /**
     *  Ball object used while animating movement
     */
    BallItem* m_movingBall;
    /**
     *  findPath() algorithm stores found path in this variable
     */
    QList<FieldPos> m_foundPath;
    /**
     *  Balls for which 'remove' animation is played
     */
    QList<BallItem*> m_removedBalls;
    /**
     *  Balls for which 'born' animation is played
     */
    QList<BallItem*> m_bornBalls;
};

#endif
