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
#include "preview.h"
#include "renderer.h"

#include <QPainter>

BallsPreview::BallsPreview( QWidget* parent )
    : QWidget(parent)
{
}

QSize BallsPreview::sizeHint() const
{
    return QSize( KLinesRenderer::self()->cellSize(), KLinesRenderer::self()->cellSize() * 3 );
}

void BallsPreview::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    QPixmap pix = KLinesRenderer::self()->backgroundTilePixmap();
    int height = pix.size().height();
    for(int i=0; i < 3; i++)
    {
        p.drawPixmap( 0, i*height, pix );
        if( !m_colors.isEmpty() )
            p.drawPixmap( 2, i*height, KLinesRenderer::self()->ballPixmap( m_colors.at(i) ) );
    }
}
