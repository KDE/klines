/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "renderer.h"
#include "klines_debug.h"
#include <KGameRenderer>
#include <KgThemeProvider>


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

KLinesRenderer  *g_KLinesRenderer = nullptr;

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
    provider->discoverThemes("appdata", QStringLiteral("themes"));
    //the default theme is marked with a key "Default=true"
    const auto themes = provider->themes();
    for (const KgTheme* theme : themes) {
        if (theme->customData(QStringLiteral("Default")) == QLatin1String("true"))
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
        qCDebug(KLINES_LOG) << "Move animation type isn't supposed to be handled by KLinesRenderer!";
        return QString();
    default:
        qCDebug(KLINES_LOG) << "Warning! Animation type not handled in switch!";
        return QString();
    }
}

QPixmap KLinesRenderer::backgroundTilePixmap()
{
    return getPixmap(QStringLiteral("field_cell"));
}

QPixmap KLinesRenderer::backgroundPixmap(QSize size)
{
    return getPixmap(QStringLiteral("background"), size);
}

QPixmap KLinesRenderer::previewPixmap()
{
    return getPixmap(QStringLiteral("preview"), QSize(m_cellSize, m_cellSize * 3));
}

bool KLinesRenderer::loadTheme()
{
    const KgTheme* theme = m_renderer->theme();

    m_numBornFrames = theme->customData(QStringLiteral("NumBornFrames")).toInt();
    m_numSelFrames = theme->customData(QStringLiteral("NumSelectedFrames")).toInt();
    m_numDieFrames = theme->customData(QStringLiteral("NumDieFrames")).toInt();

    m_bornDuration = theme->customData(QStringLiteral("BornAnimDuration")).toInt();
    m_selDuration = theme->customData(QStringLiteral("SelectedAnimDuration")).toInt();
    m_dieDuration = theme->customData(QStringLiteral("DieAnimDuration")).toInt();
    m_moveDuration = theme->customData(QStringLiteral("MoveAnimDuration")).toInt();

    return true;
}

void KLinesRenderer::setCellSize(int cellSize)
{
    if (m_cellSize == cellSize)
        return;

    m_cellSize = cellSize;
}

QPixmap KLinesRenderer::getPixmap(const QString& svgName, QSize customSize)
{
    if (m_cellSize == 0)
        return QPixmap();

    QSize sz = customSize.isValid() ? customSize : cellExtent();

    QPixmap pix = m_renderer->spritePixmap(svgName, sz);

    return pix;
}

QPixmap KLinesRenderer::backgroundBorderPixmap(QSize size)
{
    if (!hasBorderElement())
        return QPixmap();

    return getPixmap(QStringLiteral("border"), size);
}

bool KLinesRenderer::hasBorderElement()
{
    return m_renderer->spriteExists(QStringLiteral("border"));

}
