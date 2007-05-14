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

#include <KStandardDirs>
#include <KSvgRenderer>
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <KMessageBox>
#include <KLocale>

#include <QPainter>

static const int THEME_FORMAT_VERSION=1;

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
    : m_cellSize(32),
      m_numBornFrames(0), m_numSelFrames(0), m_numDieFrames(0),
      m_bornDuration(0), m_selDuration(0), m_dieDuration(0),
      m_moveDuration(0)
{
    m_renderer = new KSvgRenderer();
    if ( !loadTheme( "default" ) )
    {
        KMessageBox::error( 0,  i18n( "Failed to load default theme. Please check your installation." ) );
    }

    rerenderPixmaps();
}

KLinesRenderer::~KLinesRenderer()
{
    delete m_renderer;
}

QPixmap KLinesRenderer::ballPixmap(BallColor color) const
{
    QString id = QString( "%1_rest" ).arg( color2char( color ) );
    return m_pixHash.value( id );
}

QPixmap KLinesRenderer::animationFrame( AnimationType type, BallColor color, int frame ) const
{
    QString id;
    switch( type )
    {
    case Born:
        id = QString( "%1_born_%2" ).arg( color2char( color ) ).arg( frame+1 );
        return m_pixHash.value( id );
    case Selected:
        id = QString( "%1_select_%2" ).arg( color2char( color ) ).arg( frame+1 );
        return m_pixHash.value( id );
    case Die:
        id = QString( "%1_die_%2" ).arg( color2char( color ) ).arg( frame+1 );
        return m_pixHash.value( id );
    default:
        kDebug() << "Warning! Animation type not handled in switch!" << endl;
        return QPixmap();
    }
}

QPixmap KLinesRenderer::backgroundTilePixmap() const
{
    return m_pixHash.value( "field_cell" );
}

QPixmap KLinesRenderer::backgroundPixmap( const QSize& sz ) const
{
    if ( m_bkgnd.size() != sz )
    {
        m_bkgnd = QPixmap( sz );
        m_bkgnd.fill( Qt::transparent );
        QPainter p( &m_bkgnd );
        m_renderer->render( &p, "background" );
    }
    return m_bkgnd;
}

QPixmap KLinesRenderer::previewPixmap() const
{
    return m_pixHash.value( "preview" );
}

void KLinesRenderer::rerenderPixmaps()
{
    // this should be in sync with svg
    QString items="rbgpyec";
    QString id;

    QPainter p;
    for ( int i=0; i<items.size(); ++i )
    {
        // rendering born frames
        for ( int f=0; f<numBornFrames();f++ )
        {
            id = QString( "%1_born_%2" ).arg( items.at(i) ).arg( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
        // rendering "selected" frames
        for ( int f=0; f<numSelectedFrames();f++ )
        {
            id = QString( "%1_select_%2" ).arg( items.at(i) ).arg( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
        // rendering "die" frames
        for ( int f=0; f<numDieFrames();f++ )
        {
            id = QString( "%1_die_%2" ).arg( items.at(i) ).arg( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
        // rendering "rest frame"
        id = QString( "%1_rest" ).arg( items.at( i ) );
        QPixmap pix( m_cellSize, m_cellSize );
        pix.fill( Qt::transparent );
        p.begin( &pix );
        m_renderer->render( &p, id );
        p.end();
        m_pixHash[id] = pix;
    }
    QPixmap pix( m_cellSize, m_cellSize );
    pix.fill( Qt::transparent );
    p.begin( &pix );
    m_renderer->render( &p, "field_cell" );
    p.end();
    m_pixHash["field_cell"] = pix;

    QPixmap previewPix( m_cellSize, m_cellSize * 3);
    previewPix.fill( Qt::transparent );
    p.begin( &previewPix );
    m_renderer->render( &p, "preview" );
    p.end();
    m_pixHash["preview"] = previewPix;
}

bool KLinesRenderer::loadTheme( const QString& themeName )
{
    QString fileName = KStandardDirs::locate( "appdata", themeName+".desktop" );
    if ( fileName.isEmpty() )
    {
        kDebug() << "Failed to find theme's .desktop file!" << endl;
        return false;
    }

    KConfig themeCfg( fileName, KConfig::OnlyLocal );
    KConfigGroup theme = themeCfg.group( "KLinesTheme" );

    // THEME_FORMAT_VERSION will be increased if some incompatible changes
    // will be done to theme-config file format
    if ( theme.readEntry( "VersionFormat", 0 ) != THEME_FORMAT_VERSION )
    {
        kDebug() << "Refusing to load incompatible theme!" << endl;
        return false;
    }

    kDebug() << "Loading theme: " << theme.readEntry( "Name" ) << endl;
    m_numBornFrames = theme.readEntry( "NumBornFrames", 0 );
    m_numSelFrames = theme.readEntry( "NumSelectedFrames", 0 );
    m_numDieFrames = theme.readEntry( "NumDieFrames", 0 );

    m_bornDuration = theme.readEntry( "BornAnimDuration", 300 );
    m_selDuration = theme.readEntry( "SelectedAnimDuration", 300 );
    m_dieDuration = theme.readEntry( "DieAnimDuration", 300 );
    m_moveDuration = theme.readEntry( "MoveAnimDuration", 100 );

    bool res = m_renderer->load( KStandardDirs::locate( "appdata", theme.readEntry( "SvgFile" ) ) );
    if ( !res )
        return false;

    return true;
}
