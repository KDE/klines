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
#include <kdebug.h>

#include "ballitem.h"
#include "renderer.h"

BallItem::BallItem( QGraphicsScene* parent, const KLinesRenderer* renderer )
    : QGraphicsPixmapItem( 0, parent ), m_renderer(renderer)
{
    m_color = NumColors; // = uninitialized
    m_timeLine.setCurveShape( QTimeLine::LinearCurve );
    connect(&m_timeLine, SIGNAL(frameChanged(int)), SLOT(animFrameChanged(int)) );
}

void BallItem::setColor( BallColor c )
{
    m_color = c;
    //setPixmap( m_renderer->ballPixmap(c) );
}

void BallItem::startAnimation( BallAnimationType type )
{
    m_curAnim = type;
    switch(type)
    {
        case SelectedAnimation:
            m_timeLine.setDuration(400);
            m_timeLine.setLoopCount(0);
            m_timeLine.setFrameRange(0, m_renderer->numAnimationFrames(SelectedAnimation)-1);
            // starting by going lower
            m_timeLine.setCurrentTime( m_timeLine.duration()/2 );
            break;
        case BornAnimation:
            m_timeLine.setDuration(200);
            m_timeLine.setLoopCount(1);
            m_timeLine.setFrameRange(0, (m_renderer->numAnimationFrames(BornAnimation)-1));
            break;
    }
    m_timeLine.start();
}

void BallItem::stopAnimation()
{
    m_timeLine.stop();

    // type dependant actions:
    switch(m_curAnim)
    {
        case BornAnimation:
        case SelectedAnimation:
            setPixmap( m_renderer->ballPixmap(m_color) );
            break;
    }
}

void BallItem::animFrameChanged(int frame)
{
    setPixmap(m_renderer->animationFrame( m_color, m_curAnim, frame ));
}

#include "ballitem.moc"
