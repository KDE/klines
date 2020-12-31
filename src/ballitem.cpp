/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ballitem.h"
#include "renderer.h"

#include <QGraphicsScene>

BallItem::BallItem( QGraphicsScene* parent )
  : KGameRenderedItem(KLinesRenderer::renderer() , QString(), nullptr)
{
    parent->addItem(this);
    setShapeMode( BoundingRectShape );

    m_color = NumColors; // = uninitialized

    m_timeLine.setEasingCurve(QEasingCurve::Linear);
    m_timeLine.setLoopCount(0);

    connect(&m_timeLine, &QTimeLine::frameChanged, this, &BallItem::animFrameChanged);
}

void BallItem::setColor( BallColor c, bool setPix )
{
    m_color = c;
    if(setPix)
      setSpriteKey(KLinesRenderer::ballPixmapId(m_color));
}

void BallItem::startSelectedAnimation()
{
    if(m_timeLine.state() == QTimeLine::Running)
        return;
    // it needs to be here rather than in constructor,
    // because if different theme would get selected
    // new settings will be picked up from KLinesRenderer
    m_timeLine.setDuration(KLinesRenderer::animDuration(KLinesRenderer::SelectedAnim));
    m_timeLine.setFrameRange(0, KLinesRenderer::frameCount(KLinesRenderer::SelectedAnim)-1);
    m_timeLine.start();
}

void BallItem::stopAnimation()
{
    m_timeLine.stop();
    setSpriteKey(KLinesRenderer::ballPixmapId(m_color));
}



void BallItem::animFrameChanged(int frame)
{
  setSpriteKey(KLinesRenderer::animationFrameId(KLinesRenderer::SelectedAnim, m_color, frame ));
}


