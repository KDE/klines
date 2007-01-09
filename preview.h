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
#ifndef KL_PREVIEW_H
#define KL_PREVIEW_H

#include <QWidget>
#include "commondefs.h"

/**
 *  Widget for displaying next-turn balls
 */
class BallsPreview : public QWidget
{
public:
    explicit BallsPreview( QWidget* parent );
    /**
     *  Sets 3 colors to display
     */
    void setColors( const QList<BallColor>& colorlist ) { m_colors = colorlist; update(); }
    /**
     *  Shows or hides balls display
     */
    void setShowColors( bool show ) { m_showColors = show; update(); }

    virtual QSize sizeHint() const;
private:
    void paintEvent( QPaintEvent* );

    QList<BallColor> m_colors;
    bool m_showColors;
};

#endif
