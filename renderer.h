/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
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

#include "commondefs.h"

class KSvgRenderer;
class KPixmapCache;

/**
 * This class is responsible for rendering all the game graphics.
 * Graphics is rendered from svg file specified by current theme.
 * QPixmaps which are returned are cached until setCellSize()
 * doesn't get called.
 * Only one instance of this class exists during a program run.
 * It can be accessed with static function KLinesRenderer::self().
 */
class KLinesRenderer
{
public:
    enum AnimationType { BornAnim, SelectedAnim, DieAnim, MoveAnim };
    /**
     * Returns one and the only instance of KLinesRenderer
     */
    static KLinesRenderer* self();
    /**
     * Loads theme specified in preferences or a default one if none specified.
     * Resets cache and puts new flashy rerendered pixmaps there
     */
    bool loadTheme();

    /**
     * @return pixmap of the ball of color c in steady state
     */
    QPixmap ballPixmap( BallColor c ) const;
    /**
     * @param type type of animation sequence
     * @param c color of the ball
     * @param frame frame number (must be between 0..numFrames(type)-1)
     * @return pixmap containing animation frame
     */
    QPixmap animationFrame( AnimationType type, BallColor c, int frame ) const;
    /**
     * @return pixmap for background painting.
     */
    QPixmap backgroundPixmap(const QSize& size) const;
    /**
     * @return pixmap for border surrounding the play field.
     * Will return an invalid QPixmap if no such element exists
     * in theme's svg file.
     * @see hasBorderElement
     */
    QPixmap backgroundBorderPixmap( const QSize& size ) const;
    /**
     * @return pixmap of background tile (cell)
     */
    QPixmap backgroundTilePixmap() const;
    /**
     * @return pixmap for PreviewItem
     */
    QPixmap previewPixmap() const;
    /**
     * Sets render sizes for cells
     */
    void setCellSize(int cellSize);
    /**
     * @return current cell size
     */
    int cellSize() const { return m_cellSize; }

    bool hasBorderElement() const;

    /**
     * @return number of frames in animation sequence of type t
     */
    inline int frameCount( AnimationType t ) const
        {
            switch(t)
            {
            case BornAnim:
                return m_numBornFrames;
            case SelectedAnim:
                return m_numSelFrames;
            case DieAnim:
                return m_numDieFrames;
            default: // e.g. Move - not handled here
                return 0;
            }
        }
    /**
     * @return duration of animation sequence of type t
     */
    inline int animDuration(AnimationType t) const
        {
            switch(t)
            {
            case BornAnim:
                return m_bornDuration;
            case SelectedAnim:
                return m_selDuration;
            case DieAnim:
                return m_dieDuration;
            case MoveAnim:
                return m_moveDuration;
            default:
                return 0;
            }
        }
private:
    // disable copy - it's singleton
    KLinesRenderer();
    KLinesRenderer( const KLinesRenderer& );
    KLinesRenderer& operator=( const KLinesRenderer& );
    ~KLinesRenderer();

    /**
     * Tries to find pixmap with cacheName in cache.
     * If pixmap is not found in cache, this function will put it there
     * Pixmap is rendered according to current cellSize
     * If customSize is not passed, pixmap will be of (m_cellSize,m_cellSize) size
     *
     * @return rendered pixmap
     */
    QPixmap pixmapFromCache(const QString& svgName, const QSize& customSize = QSize()) const;

    /**
     * Searches for "Default=true" keyword in all of the theme's desktop files in theme folder
     * and return this theme name in format like "themes/theme.desktop"
     */
    QString findDefaultThemeName() const;

    /**
     *  This is the size of the scene's cell.
     *  All rendered pixmaps (except background) will have this size
     */
    int m_cellSize;
    /**
     * Name of currently loaded theme
     */
    QString m_currentTheme;

    KSvgRenderer *m_renderer;
    KPixmapCache *m_cache;

    int m_numBornFrames;
    int m_numSelFrames;
    int m_numDieFrames;

    int m_bornDuration;
    int m_selDuration;
    int m_dieDuration;
    int m_moveDuration; // one cell
};

#endif
