/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "previewitem.h"
#include "renderer.h"

#include <QPainter>

PreviewItem::PreviewItem( QGraphicsScene* scene )
    : QGraphicsItem( nullptr )
{
    Q_UNUSED( scene );
}

void PreviewItem::paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* )
{
    int ballHeight = KLinesRenderer::cellSize();
    p->drawPixmap( 0, 0, KLinesRenderer::previewPixmap() );
    for(int i=0; i < 3; i++)
        p->drawPixmap( 2, i*ballHeight,
                       KLinesRenderer::ballPixmap( m_colors.at(i) ) );
}

QRectF PreviewItem::boundingRect() const
{
    return KLinesRenderer::previewPixmap().rect();
}

void PreviewItem::setPreviewColors( const QList<BallColor>& colors )
{
    m_colors = colors;
    update();
}
