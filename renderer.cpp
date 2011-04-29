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

#include <KGameRenderer>
#include <QSvgRenderer>
#include <KDebug>
#include <KGameTheme>
#include <KStandardDirs>

#include <QPainter>
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
QString KLinesRenderer::m_currentTheme;

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
    QString themeName = Prefs::theme();
    // if no theme is specified load default one
    if (themeName.isEmpty())
        themeName = findDefaultThemeName();
    m_renderer = new KGameRenderer(themeName);

    if (!loadTheme())
        kDebug() << "Failed to load theme" << Prefs::theme();
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
    QString themeName = Prefs::theme();
    // if no theme is specified load default one
    if (themeName.isEmpty()) {
        themeName = findDefaultThemeName();
        if (themeName.isEmpty()) {
            kDebug() << "Error: failed to load default theme";
            return false;
        }
    }

    if (!m_currentTheme.isEmpty() && m_currentTheme == themeName) {
        kDebug() << "Notice: not loading the same theme";
        return true; // this is not an error
    }
    KGameTheme theme;
    if (!theme.load(themeName)) {
        kDebug() << "Failed to load theme" << themeName;
        kDebug() << "Trying to load default";
        // clear theme name and try again
        Prefs::setTheme(QString());
        return loadTheme();
    }

    m_currentTheme = themeName;
    m_renderer->setTheme(themeName);

    m_numBornFrames = theme.property(QLatin1String("NumBornFrames")).toInt();
    m_numSelFrames = theme.property(QLatin1String("NumSelectedFrames")).toInt();
    m_numDieFrames = theme.property(QLatin1String("NumDieFrames")).toInt();

    m_bornDuration = theme.property(QLatin1String("BornAnimDuration")).toInt();
    m_selDuration = theme.property(QLatin1String("SelectedAnimDuration")).toInt();
    m_dieDuration = theme.property(QLatin1String("DieAnimDuration")).toInt();
    m_moveDuration = theme.property(QLatin1String("MoveAnimDuration")).toInt();

    return true;
}

QString KLinesRenderer::findDefaultThemeName()
{
    QStringList themeDesktopFiles = KGlobal::dirs()->findAllResources("appdata", QLatin1String("themes/*.desktop"));

    QString defaultThemeName;

    foreach(const QString & file, themeDesktopFiles) {
        KConfig cfg(file, KConfig::SimpleConfig);
        KConfigGroup cfgGrp(&cfg, "KGameTheme");
        bool isDefault = cfgGrp.readEntry("Default", false);
        if (isDefault) {
            QFileInfo fi(file);
            defaultThemeName = QLatin1String("themes/") + fi.fileName();
            kDebug() << "found default theme:" << defaultThemeName;
        }
    }

    // if not found fallback to themes/default.desktop
    if (defaultThemeName.isEmpty())
        kDebug() << "didn't find default theme specification.";

    return defaultThemeName;
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
