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
#include <KGameTheme>
#include <KPixmapCache>
#include <KStandardDirs>

#include <QPainter>
#include <QFileInfo>

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
    m_cache->setCacheLimit(3*1024);

    if ( !loadTheme() )
        kDebug()<< "Failed to load theme" << Prefs::theme();
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
        kDebug() << "Move animation type isn't supposed to be handled by KLinesRenderer!";
        return QPixmap();
    default:
        kDebug() << "Warning! Animation type not handled in switch!";
        return QPixmap();
    }
}

QPixmap KLinesRenderer::backgroundTilePixmap() const
{
    return pixmapFromCache( "field_cell" );
}

QPixmap KLinesRenderer::backgroundPixmap( const QSize& size ) const
{
    return pixmapFromCache( "background", size );
}

QPixmap KLinesRenderer::previewPixmap() const
{
    return pixmapFromCache( "preview", QSize(m_cellSize, m_cellSize*3) );
}

bool KLinesRenderer::loadTheme()
{
    QString themeName = Prefs::theme();
    // if no theme is specified load default one
    if (themeName.isEmpty())
    {
        themeName = findDefaultThemeName();
        if (themeName.isEmpty())
        {
            kDebug() << "Error: failed to load default theme";
            return false;
        }
    }
    // variable saying whether to discard old cache upon successful new theme loading
    // we won't discard it if m_currentTheme is empty meaning that
    // this is the first time loadTheme() is called
    // (i.e. during startup) as we want to pick the cache from disc
    bool discardCache = !m_currentTheme.isEmpty();

    if( !m_currentTheme.isEmpty() && m_currentTheme == themeName )
    {
        kDebug() << "Notice: not loading the same theme";
        return true; // this is not an error
    }
    KGameTheme theme;
    if ( !theme.load( themeName ) )
    {
        kDebug()<< "Failed to load theme" << Prefs::theme();
        kDebug() << "Trying to load default";
        if(!theme.loadDefault())
            return false;
    }

    m_currentTheme = themeName;

    bool res = m_renderer->load( theme.graphics() );
    kDebug() << "loading" << theme.graphics();
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
        kDebug() << "discarding cache";
        m_cache->discard();
    }

    return true;
}

QString KLinesRenderer::findDefaultThemeName() const
{
    QStringList themeDesktopFiles = KGlobal::dirs()->findAllResources("appdata", "themes/*.desktop");

    QString defaultThemeName;

    foreach (const QString& file, themeDesktopFiles)
    {
        KConfig cfg(file, KConfig::SimpleConfig);
        KConfigGroup cfgGrp(&cfg, "KGameTheme");
        bool isDefault = cfgGrp.readEntry("Default", false);
        if (isDefault)
        {
            QFileInfo fi(file);
            defaultThemeName = "themes/"+fi.fileName();
            kDebug() << "found default theme:" << defaultThemeName;
        }
    }

    // if not found fallback to themes/default.desktop
    if (defaultThemeName.isEmpty())
    {
        kDebug() << "didn't find default theme specification. falling back to themes/default.desktop";
        defaultThemeName = "themes/default.desktop";
    }

    return defaultThemeName;
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
    QSize sz = customSize.isValid() ? customSize : QSize(m_cellSize,m_cellSize);

    QString cacheName = svgName+QString("_%1x%2").arg(sz.width()).arg(sz.height());
    if(!m_cache->find(cacheName, pix))
    {
//        kDebug() << "putting" << cacheName << "to cache";
        pix = QPixmap( sz );
        pix.fill( Qt::transparent );
        QPainter p( &pix );
        m_renderer->render( &p, svgName );
        p.end();
        m_cache->insert(cacheName, pix);
    }
    return pix;
}

QPixmap KLinesRenderer::backgroundBorderPixmap( const QSize& size ) const
{
    if( !hasBorderElement() )
        return QPixmap();

    return pixmapFromCache( "border", size );
}

bool KLinesRenderer::hasBorderElement() const
{
    return m_renderer->elementExists( "border" );

}
