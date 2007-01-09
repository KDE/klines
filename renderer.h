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
#ifndef KL_RENDERER_H
#define KL_RENDERER_H

#include <QPixmap>

#include "commondefs.h"

// FIXME dimsuz: give {fire,born,selected}Pixmap methods better names
class KLinesRenderer
{
public:
    static KLinesRenderer* self();

    QPixmap ballPixmap( BallColor c ) const;
    QPixmap firePixmap( int frame ) const;
    QPixmap bornPixmap( BallColor c, int frame ) const;
    QPixmap selectedPixmap( BallColor c, int frame ) const;
    QPixmap backgroundTilePixmap() const;

    void setCellSize(int size) { m_cellSize = size; }
    int cellSize() const { return m_cellSize; }

    inline int numFireFrames() const { return 5; }
    inline int numBornFrames() const { return 5; }
    inline int numSelectedFrames() const { return 13; }
private:
    // disable copy - it's singleton
    KLinesRenderer();
    KLinesRenderer( const KLinesRenderer& );
    KLinesRenderer& operator=( const KLinesRenderer& );
    ~KLinesRenderer() { }

    QPixmap m_ballsPix; // to be removed when SVG comes to us
    QPixmap m_fieldPix; // to be removed when SVG comes to us
    QPixmap m_firePix; // to be removed when SVG comes to us

    /**
     *  This is the size of the scene's cell.
     *  All rendered pixmaps will have this size
     */
    int m_cellSize;
};

#endif
