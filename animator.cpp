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

KLinesAnimator::KLinesAnimator( KLinesScene* scene )
    : m_scene(scene), m_movingBall(0)
{
    connect(&m_timeLine, SIGNAL(frameChanged(int)), SLOT(animFrameChanged(int)) );
    connect(&m_timeLine, SIGNAL(finished()), SIGNAL(moveFinished()));
}

void KLinesAnimator::animateMove( const FieldPos& from, const FieldPos& to )
{
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
