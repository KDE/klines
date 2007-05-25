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

#include <KGameTheme>

#include <QPainter>

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
    m_theme = new KGameTheme();

    if ( !loadTheme("default") )
    {
        KMessageBox::error( 0,  i18n( "Failed to load default theme. Please check your installation." ) );
    }

//    rerenderPixmaps();
}

KLinesRenderer::~KLinesRenderer()
{
    delete m_renderer;
    delete m_theme;
}

QPixmap KLinesRenderer::ballPixmap(BallColor color) const
{
    QString id = color2char( color )+QString( "_rest" );
    return m_pixHash.value( id );
}

QPixmap KLinesRenderer::animationFrame( AnimationType type, BallColor color, int frame ) const
{
    QString id;
    switch( type )
    {
    case Born:
        id = color2char( color )+QString( "_born_" ) + QString::number( frame+1 );
        return m_pixHash.value( id );
    case Selected:
        id = color2char( color )+QString( "_select_" ) + QString::number( frame+1 );
        return m_pixHash.value( id );
    case Die:
        id = color2char( color )+QString( "_die_" ) + QString::number( frame+1 );
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
    const char items[]="rbgpyec";
    const int numItems = 7;
    QString id;

    QPainter p;

    for ( int i=0; i<numItems; ++i )
    {
        // rendering born frames
        for ( int f=0; f<numBornFrames();f++ )
        {
            id = items[i]+QString( "_born_" )+QString::number( f+1 );
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
            id = items[i]+QString( "_select_" ) + QString::number( f+1 );
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
            id = items[i]+QString( "_die_" ) + QString::number( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
        // rendering "rest frame"
        id = items[i]+QString( "_rest" );
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
    if ( !m_theme->load( "themes/"+themeName+".desktop" ) )
        return false;

    bool res = m_renderer->load( m_theme->graphics() );
    if ( !res )
        return false;

    m_numBornFrames = m_theme->property( "NumBornFrames" ).toInt();
    m_numSelFrames = m_theme->property( "NumSelectedFrames" ).toInt();
    m_numDieFrames = m_theme->property( "NumDieFrames" ).toInt();

    m_bornDuration = m_theme->property( "BornAnimDuration" ).toInt();
    m_selDuration = m_theme->property( "SelectedAnimDuration" ).toInt();
    m_dieDuration = m_theme->property( "DieAnimDuration" ).toInt();
    m_moveDuration = m_theme->property( "MoveAnimDuration" ).toInt();


    return true;
}

void KLinesRenderer::setCellSize(int size)
{
    if ( m_cellSize != size )
    {
        m_cellSize = size;
        rerenderPixmaps();
    }
}
