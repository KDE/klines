/*
    SPDX-FileCopyrightText: 2000 Roman Merzlyakov <roman@sbrf.barrt.ru>
    SPDX-FileCopyrightText: 2000 Roman Razilov <Roman.Razilov@gmx.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/*
  * Roman Razilov 2000-05-19 debug dummmy
  * Roman Razilov 2000-05-21 qimgio
*/

#include <QApplication>
#include <QCommandLineParser>

#include <KLocalizedString>
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
#include "klines.h"
#include "klines_version.h"


int main( int argc, char **argv )
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("klines"));
    
    KAboutData aboutData(QStringLiteral("klines"), i18n("Kolor Lines"), QStringLiteral(KLINES_VERSION_STRING),
                         i18n("Kolor Lines - a little game about balls and how to get rid of them"),
                         KAboutLicense::GPL,
                         i18n("(c) 2000-2015 The KLines Authors"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/klines"));
    aboutData.addAuthor(i18n("Roman Merzlyakov"), i18n("Original author"), QStringLiteral("roman@sbrf.barrt.ru"));
    aboutData.addAuthor(i18n("Roman Razilov"), i18n("Rewrite and Extension"), QStringLiteral("Roman.Razilov@gmx.de"));
    aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Rewrite to use QGraphicsView. Current maintainer"), QStringLiteral("dimsuz@gmail.com"));
    aboutData.addCredit(i18n("Eugene Trounev"), i18n("New SVG artwork for KDE4 version of the game"), QStringLiteral("eugene.trounev@gmail.com"));
    
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("klines")));
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
     
    KDBusService service;    
    if (app.isSessionRestored())
        kRestoreMainWindows<KLinesMainWindow>();
    else {
        KLinesMainWindow *window = new KLinesMainWindow;
        window->show();
    }
    return app.exec();
}
