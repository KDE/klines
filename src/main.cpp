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

#include <KLocalizedString>
#include <KAboutData>
#include <KCrash>
#include <kdelibs4configmigrator.h>
#include <QCommandLineParser>
#include <KDBusService>
#include "klines.h"
#include "klines_version.h"


int main( int argc, char **argv )
{
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    Kdelibs4ConfigMigrator migrate(QStringLiteral("klines"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("klinesrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("klinesui.rc"));
    migrate.migrate();
    KLocalizedString::setApplicationDomain("klines");
    
    KAboutData aboutData(QStringLiteral("klines"), i18n("Kolor Lines"), QStringLiteral(KLINES_VERSION_STRING),
                         i18n("Kolor Lines - a little game about balls and how to get rid of them"),
                         KAboutLicense::GPL,
                         i18n("(c) 2000-2015 The KLines Authors"), QString(), QStringLiteral("https://kde.org/applications/games/org.kde.klines"));
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
