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

#include <KDebug>
#include <KGameRenderer>
#include <KgThemeProvider>

#include <QFileInfo>

// note: this should be in sync with svg
static inline char color2char(BallColor col)
{
    switch (col) {
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

int KLinesRenderer::m_cellSize = 0;
KGameRenderer *KLinesRenderer::m_renderer;
int KLinesRenderer::m_numBornFrames(0);
int KLinesRenderer::m_numSelFrames(0);
int KLinesRenderer::m_numDieFrames(0);
int KLinesRenderer::m_bornDuration(0);
int KLinesRenderer::m_selDuration(0);
int KLinesRenderer::m_dieDuration(0);
int KLinesRenderer::m_moveDuration(0);

KLinesRenderer  *g_KLinesRenderer = NULL;

void KLinesRenderer::Init()
{
    g_KLinesRenderer = new KLinesRenderer();
}

void KLinesRenderer::UnInit()
{
    delete g_KLinesRenderer;
}

KLinesRenderer::KLinesRenderer()
{
    KgThemeProvider* provider = new KgThemeProvider;
    provider->discoverThemes("appdata", QLatin1String("themes"));
    //the default theme is marked with a key "Default=true"
    foreach (const KgTheme* theme, provider->themes())
    {
        if (theme->customData(QLatin1String("Default")) == QLatin1String("true"))
        {
            provider->setDefaultTheme(theme);
            break;
        }
    }
    m_renderer = new KGameRenderer(provider);
    loadTheme();
}

KLinesRenderer::~KLinesRenderer()
{
    delete m_renderer;
}

QString KLinesRenderer::ballPixmapId(BallColor color)
{
    return QLatin1Char(color2char(color)) + QLatin1String("_rest");
}

QPixmap KLinesRenderer::ballPixmap(BallColor color)
{
    return getPixmap(ballPixmapId(color));
}

QString KLinesRenderer::animationFrameId(AnimationType type, BallColor color, int frame)
{
    switch (type) {
    case BornAnim:
        return QLatin1Char(color2char(color)) + QLatin1String("_born_") + QString::number(frame + 1);
    case SelectedAnim:
        return QLatin1Char(color2char(color)) + QLatin1String("_select_") + QString::number(frame + 1);
    case DieAnim:
        return QLatin1Char(color2char(color)) + QLatin1String("_die_") + QString::number(frame + 1);
    case MoveAnim:
        kDebug() << "Move animation type isn't supposed to be handled by KLinesRenderer!";
        return QString();
    default:
        kDebug() << "Warning! Animation type not handled in switch!";
        return QString();
    }
}

QPixmap KLinesRenderer::backgroundTilePixmap()
{
    return getPixmap(QLatin1String("field_cell"));
}

QPixmap KLinesRenderer::backgroundPixmap(const QSize& size)
{
    return getPixmap(QLatin1String("background"), size);
}

QPixmap KLinesRenderer::previewPixmap()
{
    return getPixmap(QLatin1String("preview"), QSize(m_cellSize, m_cellSize * 3));
}

bool KLinesRenderer::loadTheme()
{
    const KgTheme* theme = m_renderer->theme();

    m_numBornFrames = theme->customData(QLatin1String("NumBornFrames")).toInt();
    m_numSelFrames = theme->customData(QLatin1String("NumSelectedFrames")).toInt();
    m_numDieFrames = theme->customData(QLatin1String("NumDieFrames")).toInt();

    m_bornDuration = theme->customData(QLatin1String("BornAnimDuration")).toInt();
    m_selDuration = theme->customData(QLatin1String("SelectedAnimDuration")).toInt();
    m_dieDuration = theme->customData(QLatin1String("DieAnimDuration")).toInt();
    m_moveDuration = theme->customData(QLatin1String("MoveAnimDuration")).toInt();

    return true;
}

void KLinesRenderer::setCellSize(int cellSize)
{
    if (m_cellSize == cellSize)
        return;

    m_cellSize = cellSize;
}

QPixmap KLinesRenderer::getPixmap(const QString& svgName, const QSize& customSize)
{
    if (m_cellSize == 0)
        return QPixmap();

    QSize sz = customSize.isValid() ? customSize : cellExtent();

    QPixmap pix = m_renderer->spritePixmap(svgName, sz);

    return pix;
}

QPixmap KLinesRenderer::backgroundBorderPixmap(const QSize& size)
{
    if (!hasBorderElement())
        return QPixmap();

    return getPixmap(QLatin1String("border"), size);
}

bool KLinesRenderer::hasBorderElement()
{
    return m_renderer->spriteExists(QLatin1String("border"));

}
