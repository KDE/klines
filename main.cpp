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


#include <kapplication.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "klines.h"

static const char description[] = I18N_NOOP("Kolor Lines - a little game about balls and how to get rid of them");

// A hack to circumvent tricky i18n issue, not used later on in the code.
// Both context and contents must be exactly the same as for the entry in
// kdelibs/kdeui/ui_standards.rc
static const char dummy[] = I18N_NOOP2("Menu title", "&Move");

int main( int argc, char **argv )
{
	KAboutData aboutData("klines", I18N_NOOP("Kolor Lines"), LINESVERSION,
                         description, KAboutData::License_GPL);
    aboutData.addAuthor("Roman Merzlyakov", I18N_NOOP("Original author"), "roman@sbrf.barrt.ru");
    aboutData.addAuthor("Roman Razilov", I18N_NOOP("Rewrite and Extension"), "Roman.Razilov@gmx.de");
    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication a;
    KGlobal::locale()->insertCatalogue("libkdegames");

    if (a.isRestored())
        RESTORE(KLines)
    else {
        KLines *w = new KLines;
        a.setMainWidget(w);
        w->show();
    }
    return a.exec();
}
