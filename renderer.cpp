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
#include <KStandardDirs>

#include "renderer.h"

KLinesRenderer* KLinesRenderer::self()
{
    static KLinesRenderer instance;
    return &instance;
}

KLinesRenderer::KLinesRenderer()
    : m_cellSize(32)
{
    m_fieldPix = QPixmap( KStandardDirs::locate( "appdata", "field.jpg" ));
    m_firePix = QPixmap( KStandardDirs::locate( "appdata", "fire.jpg" ));
    m_ballsPix = QPixmap( KStandardDirs::locate( "appdata", "balls.jpg" ));
}

// these are the notes for all *Pixmap() functions below:
// FIXME dimsuz: copying every time: not very efficient.
// FIXME dimsuz: hardcoded "magic" numbers
// FIXME dimsuz: scale to emulate future svg support
// Switching to svg will make this fixmes obsolete
    
QPixmap KLinesRenderer::ballPixmap(BallColor color) const
{
    // col, row, width, height - hardcoded. balls.jpg has such a format.
    return m_ballsPix.copy( 7*30, static_cast<int>(color)*30, 30, 30 ).scaled( m_cellSize, m_cellSize );
}

QPixmap KLinesRenderer::firePixmap(int frame) const
{
    // col, row, width, height - hardcoded. balls.jpg has such a format.
    return m_firePix.copy(frame*30, 0, 30, 30 ).scaled( m_cellSize, m_cellSize );
}

QPixmap KLinesRenderer::bornPixmap(BallColor color, int frame) const
{
    // col, row, width, height - hardcoded. balls.jpg has such a format.
    return m_ballsPix.copy( 13*30 + frame*30, static_cast<int>(color)*30, 30, 30 ).scaled( m_cellSize, m_cellSize );
}

QPixmap KLinesRenderer::selectedPixmap( BallColor color, int frame ) const
{
    return m_ballsPix.copy( frame*30, static_cast<int>(color)*30, 30, 30 ).scaled( m_cellSize, m_cellSize );
}

QPixmap KLinesRenderer::backgroundTilePixmap() const
{
    return m_fieldPix.scaled( m_cellSize, m_cellSize );
}
