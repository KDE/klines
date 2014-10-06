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

#include "klines.h"

static const char description[] = I18N_NOOP("Kolor Lines - a little game about balls and how to get rid of them");

int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    
    KAboutData aboutData(QStringLiteral("klines"), i18n("Kolor Lines"), QStringLiteral("1.5"),
                         i18n(description), KAboutLicense::GPL,
                         i18n("(c) 2000-2008 The KLines Authors"), QString(), QStringLiteral("http://games.kde.org/klines"));
    aboutData.addAuthor(i18n("Roman Merzlyakov"), i18n("Original author"), QStringLiteral("roman@sbrf.barrt.ru"));
    aboutData.addAuthor(i18n("Roman Razilov"), i18n("Rewrite and Extension"), QStringLiteral("Roman.Razilov@gmx.de"));
    aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Rewrite to use QGraphicsView. Current maintainer"), QStringLiteral("dimsuz@gmail.com"));
    aboutData.addCredit(i18n("Eugene Trounev"), i18n("New SVG artwork for KDE4 version of the game"), QStringLiteral("eugene.trounev@gmail.com"));
    
    aboutData.setOrganizationDomain(QByteArray("kde.org"));
    aboutData.setProgramIconName(QStringLiteral("klines"));
    aboutData.setProductName(QByteArray("klines"));
    
    KAboutData::setApplicationData(aboutData);
        
    app.setApplicationDisplayName(aboutData.displayName());
    app.setOrganizationDomain(aboutData.organizationDomain());
    app.setApplicationVersion(aboutData.version());
    
    KLocalizedString::setApplicationDomain("klines");
    
    if (app.isSessionRestored())
        RESTORE(KLinesMainWindow)
    else {
        KLinesMainWindow *window = new KLinesMainWindow;
        window->show();
    }
    return app.exec();
}
