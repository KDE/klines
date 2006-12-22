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
    connect(&m_timeLine, SIGNAL(frameChanged(int)), SLOT(animFrameChanged(int)) );
}

void BallItem::setColor( BallColor c )
{
    m_color = c;
    setPixmap( m_renderer->ballPixmap(c) );
}

void BallItem::startAnimation( BallAnimationType type )
{
    m_curAnim = type;
    switch(type)
    {
        case SelectedAnimation:
            m_timeLine.setDuration(400);
            m_timeLine.setLoopCount(0);
            m_timeLine.setCurveShape( QTimeLine::LinearCurve );
            // in a balls.jpg we have 9 effective frames
            // of raising the ball up. lowing down frames are the
            // same 9 frames only played in opposite direction.
            // So I setup timeline for 0-18 frame range and in animFrameChanged()
            // i'll make sure that after frame number will go past 9 it
            // will be recalculated to be 8,7,6,...,0 etc
            m_timeLine.setFrameRange(0, 9*2);
            // starting by going lower
            m_timeLine.setCurrentTime( m_timeLine.duration()/2 );
            break;
    }
    m_timeLine.start();
}

void BallItem::stopAnimation()
{
    switch(m_curAnim)
    {
        case SelectedAnimation:
            m_timeLine.stop();
            setPixmap( m_renderer->ballPixmap(m_color) );
            break;
    }
}

void BallItem::animFrameChanged(int frame)
{
    switch(m_curAnim)
    {
        case SelectedAnimation:
            // if frame > 9, make it go in opposite direction
            // See also comment in startAnimation()
            if(frame > 9)
                frame = frame - (frame-9)*2;
            setPixmap(m_renderer->animationFrame( m_color, m_curAnim, frame ));
            break;
    }
}

#include "ballitem.moc"
