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
#ifndef KL_RENDERER_H
#define KL_RENDERER_H

#include <QPixmap>
#include <QHash>

#include "commondefs.h"

class KSvgRenderer;

// TODO documentation!
class KLinesRenderer
{
public:
    enum AnimationType { Born, Selected, Die };

    static KLinesRenderer* self();

    bool loadTheme( const QString& themeName );

    QPixmap ballPixmap( BallColor c ) const;
    QPixmap animationFrame( AnimationType type, BallColor c, int frame ) const;
    QPixmap backgroundPixmap() const;
    QPixmap backgroundTilePixmap() const;
    QPixmap previewPixmap() const;

    void setRenderSizes(int cellSize, const QSize& bkgndSize);
    int cellSize() const { return m_cellSize; }

    inline int numDieFrames() const { return m_numDieFrames; }
    inline int numBornFrames() const { return m_numBornFrames; }
    inline int numSelectedFrames() const { return m_numSelFrames; }

    inline int bornAnimDuration() const { return m_bornDuration; }
    inline int selAnimDuration() const { return m_selDuration; }
    inline int dieAnimDuration() const { return m_dieDuration; }
    inline int moveAnimDuration() const { return m_moveDuration; }
private:
    // disable copy - it's singleton
    KLinesRenderer();
    KLinesRenderer( const KLinesRenderer& );
    KLinesRenderer& operator=( const KLinesRenderer& );
    ~KLinesRenderer();

    /**
     * Rerenders all animation frames from svg to
     * pixmaps according to m_cellSize and puts them
     * to m_pixHash
     */
    void rerenderPixmaps();
    /**
     *  This is the size of the scene's cell.
     *  All rendered pixmaps (except background) will have this size
     */
    int m_cellSize;
    /**
     * Size of the background pixmap.
     */
    QSize m_bkgndSize;

    KSvgRenderer *m_renderer;
    QHash<QString, QPixmap> m_pixHash;

    int m_numBornFrames;
    int m_numSelFrames;
    int m_numDieFrames;

    int m_bornDuration;
    int m_selDuration;
    int m_dieDuration;
    int m_moveDuration; // one cell
};

#endif
