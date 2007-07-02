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


#include <KApplication>
#include <KLocale>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KGlobal>

#include "klines.h"

static const char description[] = I18N_NOOP("Kolor Lines - a little game about balls and how to get rid of them");

// A hack to circumvent tricky i18n issue, not used later on in the code.
// Both context and contents must be exactly the same as for the entry in
// kdelibs/kdeui/ui_standards.rc
static const char dummy[] = I18N_NOOP2("Menu title", "&Move");

int main( int argc, char **argv )
{
    KAboutData aboutData("klines", 0, ki18n("Kolor Lines"), "1.5",
                         ki18n(description), KAboutData::License_GPL);
    aboutData.addAuthor(ki18n("Roman Merzlyakov"), ki18n("Original author"), "roman@sbrf.barrt.ru");
    aboutData.addAuthor(ki18n("Roman Razilov"), ki18n("Rewrite and Extension"), "Roman.Razilov@gmx.de");
    aboutData.addAuthor(ki18n("Dmitry Suzdalev"), ki18n("Rewrite to use QGraphicsView. Current maintainer"), "dimsuz@gmail.com");
    aboutData.addCredit(ki18n("Eugene Trounev"), ki18n("New SVG artwork for KDE4 version of the game"), "eugene.trounev@gmail.com");
    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication application;
    KGlobal::locale()->insertCatalog("libkdegames");

    if (application.isSessionRestored())
        RESTORE(KLinesMainWindow)
    else {
        KLinesMainWindow *window = new KLinesMainWindow;
        window->show();
    }
    return application.exec();
}
