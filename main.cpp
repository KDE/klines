/***************************************************************************
                          main.cpp  -  description
                             -------------------
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


#include <kapp.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

// #include <qimageio.h>

#include "klines.h"
void dummyMessageHandler( QtMsgType type, const char *msg )
{
}
/*
#ifndef _DBG_
#define _DBG_
#endif
*/
static const char *description = I18N_NOOP("Kolor Lines - a little game about balls and how to get rid of them");

int main( int argc, char **argv )
{
	KAboutData aboutData("klines", I18N_NOOP("Kolor Lines"), LINESVERSION,
						  description, KAboutData::License_GPL);
//	 aboutData.addAuthor("Roman Merzlyakov", 0, "roman@sbrf.barrt.ru");
	 aboutData.addAuthor("Roman Razilov", 0, "Roman.Razilov@gmx.de");
	 KCmdLineArgs::init(argc, argv, &aboutData);

#ifndef _DBG_
  qInstallMsgHandler( dummyMessageHandler );
#endif
  KApplication a;
  KLines *v = new KLines;

  a.setMainWidget( v );
//  debug( "Thank you for running Lines, version %s", LINESVERSION );
  v->show();
  return a.exec();
}
