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
#include "ballitem.h"
#include "renderer.h"

#include <kdebug.h>

BallItem::BallItem( QGraphicsScene* parent )
    : QGraphicsPixmapItem( 0, parent )
{
    m_color = NumColors; // = uninitialized

    m_timeLine.setCurveShape( QTimeLine::LinearCurve );
    m_timeLine.setDuration(KLinesRenderer::self()->selAnimDuration());
    m_timeLine.setLoopCount(0);
    m_timeLine.setFrameRange(0, KLinesRenderer::self()->numSelectedFrames()-1);

    connect(&m_timeLine, SIGNAL(frameChanged(int)), SLOT(animFrameChanged(int)) );
}

void BallItem::setColor( BallColor c, bool setPix )
{
    m_color = c;
    if(setPix)
        setPixmap( KLinesRenderer::self()->ballPixmap( m_color ) );
}

void BallItem::startSelectedAnimation()
{
    if(m_timeLine.state() == QTimeLine::Running)
        return;
    m_timeLine.start();
}

void BallItem::stopAnimation()
{
    m_timeLine.stop();
    setPixmap( KLinesRenderer::self()->ballPixmap( m_color ) );
}

void BallItem::animFrameChanged(int frame)
{
    setPixmap(KLinesRenderer::self()->animationFrame( KLinesRenderer::Selected, m_color, frame ));
}

#include "ballitem.moc"
