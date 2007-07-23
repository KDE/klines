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
#include "renderer.h"
#include "prefs.h"

#include <KSvgRenderer>
#include <KDebug>
#include <KStandardDirs>
#include <KGameTheme>
#include <kpixmapcache.h>

#include <QPainter>

// if cache get's bigger then this (in kilobytes), discard it
static const int CACHE_LIMIT=3000;

// note: this should be in sync with svg
static inline char color2char( BallColor col )
{
    switch( col )
    {
    case Blue:
        return 'b';
    case Brown:
        return 'e';
    case Cyan:
        return 'c';
    case Green:
        return 'g';
    case Red:
        return 'r';
    case Violet:
        return 'p';
    case Yellow:
        return 'y';
    default:
        return 'x'; // error
    }
}

KLinesRenderer* KLinesRenderer::self()
{
    static KLinesRenderer instance;
    return &instance;
}

KLinesRenderer::KLinesRenderer()
    : m_cellSize(0),
      m_numBornFrames(0), m_numSelFrames(0), m_numDieFrames(0),
      m_bornDuration(0), m_selDuration(0), m_dieDuration(0),
      m_moveDuration(0)
{
    m_renderer = new KSvgRenderer();
    m_cache = new KPixmapCache("klines-cache");

    if ( !loadTheme( Prefs::theme() ) )
        kDebug()<< "Failed to load theme " << Prefs::theme() << endl;
}

KLinesRenderer::~KLinesRenderer()
{
    delete m_renderer;
    delete m_cache;
}

QPixmap KLinesRenderer::ballPixmap(BallColor color) const
{
    QString id = color2char( color )+QString( "_rest" );
    return pixmapFromCache(id);
}

QPixmap KLinesRenderer::animationFrame( AnimationType type, BallColor color, int frame ) const
{
    QString id;
    switch( type )
    {
    case BornAnim:
        id = color2char( color )+QString( "_born_" ) + QString::number( frame+1 );
        return pixmapFromCache(id);
    case SelectedAnim:
        id = color2char( color )+QString( "_select_" ) + QString::number( frame+1 );
        return pixmapFromCache(id);
    case DieAnim:
        id = color2char( color )+QString( "_die_" ) + QString::number( frame+1 );
        return pixmapFromCache(id);
    case MoveAnim:
        kDebug() << "Move animation type isn't supposed to be handled by KLinesRenderer!" << endl;
        return QPixmap();
    default:
        kDebug() << "Warning! Animation type not handled in switch!" << endl;
        return QPixmap();
    }
}

QPixmap KLinesRenderer::backgroundTilePixmap() const
{
    return pixmapFromCache( "field_cell" );
}

QPixmap KLinesRenderer::backgroundPixmap( const QSize& size ) const
{
    QPixmap bkgnd = pixmapFromCache( "background", size );
    if(m_cache->size() > CACHE_LIMIT)
    {
        kDebug() << "discarding cache - it got too big" << endl;
        m_cache->discard();
    }
    return bkgnd;
}

QPixmap KLinesRenderer::previewPixmap() const
{
    return pixmapFromCache( "preview", QSize(m_cellSize, m_cellSize*3) );
}

bool KLinesRenderer::loadTheme( const QString& themeName )
{
    // variable saying whether to discard old cache upon successful new theme loading
    // we won't discard it if m_currentTheme is empty meaning that
    // this is the first time loadTheme() is called
    // (i.e. during startup) as we want to pick the cache from disc
    bool discardCache = !m_currentTheme.isEmpty();

    if( !m_currentTheme.isEmpty() && m_currentTheme == themeName )
    {
        kDebug() << "Notice: not loading the same theme" << endl;
        return true; // this is not an error
    }
    KGameTheme theme;
    if ( !theme.load( themeName ) )
    {
        kDebug()<< "Failed to load theme " << Prefs::theme() << endl;
        kDebug() << "Trying to load default" << endl;
        if(!theme.loadDefault())
            return false;
    }

    m_currentTheme = themeName;

    bool res = m_renderer->load( theme.graphics() );
    kDebug() << "loading " << theme.graphics() << endl;
    if ( !res )
        return false;

    m_numBornFrames = theme.property( "NumBornFrames" ).toInt();
    m_numSelFrames = theme.property( "NumSelectedFrames" ).toInt();
    m_numDieFrames = theme.property( "NumDieFrames" ).toInt();

    m_bornDuration = theme.property( "BornAnimDuration" ).toInt();
    m_selDuration = theme.property( "SelectedAnimDuration" ).toInt();
    m_dieDuration = theme.property( "DieAnimDuration" ).toInt();
    m_moveDuration = theme.property( "MoveAnimDuration" ).toInt();

    if(discardCache)
    {
        kDebug() << "discarding cache" << endl;
        m_cache->discard();
    }

    return true;
}

void KLinesRenderer::setCellSize(int cellSize)
{
    if ( m_cellSize == cellSize )
        return;

    m_cellSize = cellSize;
}

QPixmap KLinesRenderer::pixmapFromCache(const QString& svgName, const QSize& customSize) const
{
    if(m_cellSize == 0)
        return QPixmap();

    QPixmap pix;
    QString cacheName = svgName+QString("_%1").arg(m_cellSize);
    if(!m_cache->find(cacheName, pix))
    {
        kDebug() << "putting " << cacheName << " to cache" << endl;
        if(customSize.isValid())
            pix = QPixmap( customSize );
        else
            pix = QPixmap( m_cellSize, m_cellSize );

        pix.fill( Qt::transparent );
        QPainter p( &pix );
        m_renderer->render( &p, svgName );
        p.end();
        m_cache->insert(cacheName, pix);
    }
    return pix;
}
