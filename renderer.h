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

class KGameRenderer;

/**
 * This class is responsible for rendering all the game graphics.
 * Graphics is rendered from svg file specified by current theme.
 * Only one instance of this class exists during a program run.
 * It can be accessed with static function KLinesRenderer::self().
 */
class KLinesRenderer
{
public:
    enum AnimationType { BornAnim, SelectedAnim, DieAnim, MoveAnim };

    static void Init();
    static void UnInit();

    static inline KGameRenderer * renderer()
    {
        return m_renderer;
    }
    
    /**
     * Loads theme specified in preferences or a default one if none specified.
     */
    static bool loadTheme();

    /**
     * @return pixmap of the ball of color c in steady state
     */
    static QString ballPixmapId(BallColor c);
    static QPixmap ballPixmap(BallColor c);
    /**
     * @param type type of animation sequence
     * @param c color of the ball
     * @param frame frame number (must be between 0..numFrames(type)-1)
     * @return string containing elementId
     */
    static QString animationFrameId(AnimationType type, BallColor c, int frame);
    /**
     * @return pixmap for background painting.
     */
    static QPixmap backgroundPixmap(const QSize& size);
    /**
     * @return pixmap for border surrounding the play field.
     * Will return an invalid QPixmap if no such element exists
     * in theme's svg file.
     * @see hasBorderElement
     */
    static QPixmap backgroundBorderPixmap(const QSize& size);
    /**
     * @return pixmap of background tile (cell)
     */
    static QPixmap backgroundTilePixmap();
    /**
     * @return pixmap for PreviewItem
     */
    static QPixmap previewPixmap();
    /**
     * Sets render sizes for cells
     */
    static void setCellSize(int cellSize);
    /**
     * @return current cell size
     */
    static inline int cellSize()
    {
        return m_cellSize;
    }

    static inline QSize cellExtent()
    {
        return QSize(m_cellSize, m_cellSize);
    }

    static bool hasBorderElement();

    /**
     * @return number of frames in animation sequence of type t
     */
    static inline int frameCount(AnimationType t)
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
    static inline int animDuration(AnimationType t)
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
    KLinesRenderer(const KLinesRenderer&);
    KLinesRenderer& operator=(const KLinesRenderer&);
    ~KLinesRenderer();

    /**
     * Pixmap is rendered according to current cellSize
     * If customSize is not passed, pixmap will be of (m_cellSize,m_cellSize) size
     *
     * @return rendered pixmap
     */
    static QPixmap getPixmap(const QString& svgName, const QSize& customSize = QSize());

    /**
     *  This is the size of the scene's cell.
     *  All rendered pixmaps (except background) will have this size
     */
    static int m_cellSize;

    static KGameRenderer *m_renderer;

    static int m_numBornFrames;
    static int m_numSelFrames;
    static int m_numDieFrames;

    static int m_bornDuration;
    static int m_selDuration;
    static int m_dieDuration;
    static int m_moveDuration; // one cell
};

#endif
