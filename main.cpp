/***************************************************************************
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /*
  * Roman Razilov 2000-05-19 debug dummmy
  * Roman Razilov 2000-05-21 qimgio
*/

#include <QApplication>

#include <KLocalizedString>
#include <KAboutData>
#include <kdelibs4configmigrator.h>
#include <QCommandLineParser>
#include <KDBusService>
#include "klines.h"

static const char description[] = I18N_NOOP("Kolor Lines - a little game about balls and how to get rid of them");

int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    Kdelibs4ConfigMigrator migrate(QLatin1String("klines"));
    migrate.setConfigFiles(QStringList() << QLatin1String("klinesrc"));
    migrate.setUiFiles(QStringList() << QLatin1String("klinesui.rc"));
    migrate.migrate();
    
    KAboutData aboutData(QStringLiteral("klines"), i18n("Kolor Lines"), QStringLiteral("1.6"),
                         i18n(description), KAboutLicense::GPL,
                         i18n("(c) 2000-2015 The KLines Authors"), QString(), QStringLiteral("http://games.kde.org/klines"));
    aboutData.addAuthor(i18n("Roman Merzlyakov"), i18n("Original author"), QStringLiteral("roman@sbrf.barrt.ru"));
    aboutData.addAuthor(i18n("Roman Razilov"), i18n("Rewrite and Extension"), QStringLiteral("Roman.Razilov@gmx.de"));
    aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Rewrite to use QGraphicsView. Current maintainer"), QStringLiteral("dimsuz@gmail.com"));
    aboutData.addCredit(i18n("Eugene Trounev"), i18n("New SVG artwork for KDE4 version of the game"), QStringLiteral("eugene.trounev@gmail.com"));
    
    app.setWindowIcon(QIcon::fromTheme(QLatin1String("klines")));
    KAboutData::setApplicationData(aboutData);
   
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
     
    KDBusService service;    
    if (app.isSessionRestored())
        RESTORE(KLinesMainWindow)
    else {
        KLinesMainWindow *window = new KLinesMainWindow;
        window->show();
    }
    return app.exec();
}
