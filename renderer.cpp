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
    : m_cellSize(0),
      m_numBornFrames(0), m_numSelFrames(0), m_numDieFrames(0),
      m_bornDuration(0), m_selDuration(0), m_dieDuration(0),
      m_moveDuration(0)
{
    m_renderer = new KSvgRenderer();

    if ( !loadTheme( Prefs::theme() ) )
        kDebug()<< "Failed to load theme " << Prefs::theme() << endl;
//    rerenderPixmaps();
}

KLinesRenderer::~KLinesRenderer()
{
    delete m_renderer;
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
    case BornAnim:
        id = color2char( color )+QString( "_born_" ) + QString::number( frame+1 );
        return m_pixHash.value( id );
    case SelectedAnim:
        id = color2char( color )+QString( "_select_" ) + QString::number( frame+1 );
        return m_pixHash.value( id );
    case DieAnim:
        id = color2char( color )+QString( "_die_" ) + QString::number( frame+1 );
        return m_pixHash.value( id );
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
    return m_pixHash.value( "field_cell" );
}

QPixmap KLinesRenderer::backgroundPixmap() const
{
    return m_pixHash.value( "background" );
}

QPixmap KLinesRenderer::previewPixmap() const
{
    return m_pixHash.value( "preview" );
}

void KLinesRenderer::rerenderPixmaps()
{
    // don't try render if the sizes aren't set yet
    if ( m_cellSize == 0 )
        return;

    // this should be in sync with svg
    const char items[]="rbgpyec";
    const int numItems = 7;
    QString id;

    QPainter p;

    for ( int i=0; i<numItems; ++i )
    {
        // rendering born frames
        for ( int f=0; f<frameCount(BornAnim);f++ )
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
        for ( int f=0; f<frameCount(SelectedAnim);f++ )
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
        for ( int f=0; f<frameCount(DieAnim);f++ )
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

    QPixmap bkgndPix( m_bkgndSize );
    bkgndPix.fill( Qt::transparent );
    p.begin( &bkgndPix );
    m_renderer->render( &p, "background" );
    p.end();
    m_pixHash["background"] = bkgndPix;
}

bool KLinesRenderer::loadTheme( const QString& themeName )
{
    KGameTheme theme;
    if ( !theme.load( themeName ) )
        return false;

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

    rerenderPixmaps();

    return true;
}

void KLinesRenderer::setRenderSizes(int cellSize, const QSize& bkgndSize)
{
    if ( m_cellSize == cellSize && m_bkgndSize == bkgndSize )
        return;

    m_cellSize = cellSize;
    m_bkgndSize = bkgndSize;
    rerenderPixmaps();
}
