/***************************************************************************
                          klines.cpp  -  description
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
/* changes
21.05.2000    Roman Razilov     Menu game/Next
*/
//
// The implementation of the KLines widget
//

#include <qkeycode.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qstring.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kpopupmenu.h>


#include "cfg.h"
#include <kstatusbar.h>
#include "klines.moc"


/*
   Creates the KLines widget and sets saved options (if any).
*/

KLines::KLines() : KMainWindow()
{
  time_t t;
  time(&t);
  srand((unsigned int)t + getpid());

  setCaption(QString("KLines ")+LINESVERSION);

  hs = new HScore();

  mwidget = new MainWidget(this);
  setCentralWidget( mwidget );

  lsb = mwidget->GetLsb();
  lPrompt = mwidget->GetPrompt();

  menu              = new KMenuBar(this, "menu");
  game              = new QPopupMenu;
  edit 							= new QPopupMenu;
  QPopupMenu * help = helpMenu(QString(i18n("Klines"))
           + " " + LINESVERSION
           + i18n("\n\nOriginal version by Roman Merzlyakov")
           + " (roman@sbrf.barrt.ru)"
           + i18n("\n\nRewritten and extended by ")
           + "Roman Razilov"
           + " (Roman.Razilov@gmx.de)" );

  Q_CHECK_PTR( edit );
  Q_CHECK_PTR( game );
  Q_CHECK_PTR( help );
  Q_CHECK_PTR( menu );





  game->insertItem(SmallIcon("filenew"),  i18n("&New game"), this, SLOT(stopGame()), CTRL+Key_N );
  game->insertSeparator();
  game->insertItem(SmallIcon("next"),  i18n("Ne&xt"), this, SLOT(makeTurn()), Key_N );
  game->insertSeparator();
  game->insertItem( i18n("Show &highscore"), hs, SLOT(viewHighScore()), CTRL+Key_H );
  game->insertSeparator();
  idMenuPrompt = game->insertItem( i18n("&Show next"), this, SLOT(switchPrompt()), CTRL+Key_P );
  game->setCheckable(true);
  game->setItemChecked(idMenuPrompt, lPrompt->getState());
  game->insertSeparator();
  game->insertItem(SmallIcon("exit"),  i18n("&Quit"), kapp, SLOT(quit()), CTRL+Key_Q );

  idMenuUndo = edit->insertItem(SmallIcon("undo"),  i18n("Und&o"), this, SLOT(undo()), CTRL+Key_Z );

  menu->insertItem( i18n("&Game"), game );
  menu->insertItem( i18n("&Edit"), edit );
  menu->insertSeparator();
  menu->insertItem( i18n("&Help"), help );

  menu->show();

  score = 0;
  prev_score = 0;

  QString s;
  stat = new KStatusBar(this);
  s = i18n(" score: xxxx ");
  stat->insertItem(s, LSCORE);
  s = i18n(" highscore: xxxx ");
  stat->insertItem(s, LRECORD);
	stat->show();

  startGame();
}

/*
   Saves the options and destroys the KLines widget.
*/

KLines::~KLines()
{
}

/*
   Resize event of the KLines widget.
*/

void KLines::resizeEvent( QResizeEvent *e )
{
  KMainWindow::resizeEvent(e);
}

void KLines::setMinSize()
{
//  setMinimumSize( gr->wHint(), gr->hHint() + menu->height() + stat->height() +
//      tool->height() );
}

void KLines::startGame()
{
    score = 0;
    prev_score = 0;
		bUndo=TRUE;

    lsb->clearField();
    generateRandomBalls();
    placeBalls();
    generateRandomBalls();
    edit->setItemEnabled(idMenuUndo, FALSE );
    updateStat();
}
void KLines::stopGame()
{
  debug("Klines::stopGame");
  endGame();
}

void KLines::searchBallsLine()
{
}

void KLines::generateRandomBalls()
{
	
		for( int i = 0 ; i < BALLSDROP ; i++ )
		{
	    nextBalls_undo[i] = nextBalls[i];		
	    nextBalls[i] = bUndo ?
						rand() % NCOLORS:
						nextBalls_redo[i];
		}
    lPrompt->SetBalls(nextBalls);
}

void KLines::placeBalls()
{
    lsb->placeBalls(nextBalls);
    debug("exit from placeBalls");
}

void KLines::undo()
{
		debug("Undo");
		if (!bUndo)
			return;
		for( int i = 0 ; i < BALLSDROP ; i++ )
		{
	    nextBalls_redo[i] = nextBalls[i];		
	    nextBalls[i] = nextBalls_undo[i];
		}
    lPrompt->SetBalls(nextBalls);
		lsb->undo();
		switchUndo(FALSE);
}

void KLines::makeTurn()
{
    placeBalls();
    generateRandomBalls();
		switchUndo(TRUE);
}

void KLines::addScore(int ballsErased)
{   if(ballsErased >= 5){
      score += 2*ballsErased*ballsErased - 20*ballsErased + 60 ;
      if( !lPrompt->getState() ) score+= 1;
      updateStat();
    };
}

void KLines::updateStat()
{
    stat->changeItem(i18n(" score: %1 ").arg(score), LSCORE);
    stat->changeItem(i18n(" highscore: %1 ").arg(hs->getMaxScore()), LRECORD);
}

void KLines::endGame()
{
    if(score > hs->getMaxScore() ) updateStat();
    hs->slotEndOfGame(score);

    startGame();
}

void KLines::switchPrompt()
{
    lPrompt->setPrompt(!lPrompt->getState());
    game->setItemChecked(idMenuPrompt, lPrompt->getState());
}

void KLines::switchUndo(bool bu)
{
		bUndo = bu;
    edit->setItemEnabled(idMenuUndo, bUndo );
}

void KLines::help()
{
//  KApplication::getKApplication()->invokeHTMLHelp("", "");
}


