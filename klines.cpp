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
#include <kscoredialog.h>


#include "cfg.h"
#include <kstatusbar.h>
#include "klines.moc"


/*
   Creates the KLines widget and sets saved options (if any).
*/

KLines::KLines() : KMainWindow()
{
  setCaption(QString("KLines ")+LINESVERSION);

  mwidget = new MainWidget(this);
  setCentralWidget( mwidget );

  lsb = mwidget->GetLsb();
  connect(lsb, SIGNAL(endTurn()), this, SLOT(makeTurn()));
  connect(lsb, SIGNAL(eraseLine(int)), this, SLOT(addScore(int)));
  connect(lsb, SIGNAL(endGame()), this, SLOT(endGame()));

  lPrompt = mwidget->GetPrompt();

  menu              = new KMenuBar(this, "menu");
  game              = new QPopupMenu;
  edit               = new QPopupMenu;
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
  
  game->insertItem(SmallIcon("filenew"),  i18n("&New Game"), this, SLOT(stopGame()), CTRL+Key_N );
  game->insertSeparator();
  game->insertItem(SmallIcon("next"),  i18n("Ne&xt"), this, SLOT(makeTurn()), Key_N );
  game->insertSeparator();
  game->insertItem( i18n("Show &Highscore"), this, SLOT(viewHighScore()), CTRL+Key_H );
  game->insertSeparator();
  idMenuPrompt = game->insertItem( i18n("&Show Next"), this, SLOT(switchPrompt()), CTRL+Key_P );
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
  score_undo = 0;

  statusBar()->insertItem(i18n(" Score:"), 0, 1);
  statusBar()->setItemAlignment(0, AlignVCenter | AlignLeft);

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

void KLines::startGame()
{
    score = 0;
    score_undo = 0;
    bUndo=TRUE;

    lsb->setGameOver(false);
    lsb->clearField();
    generateRandomBalls();
    placeBalls();
    generateRandomBalls();
    edit->setItemEnabled(idMenuUndo, FALSE );
    updateStat();
}

void KLines::stopGame()
{
    if (!lsb->gameOver())
       endGame();
    startGame();
}

void KLines::searchBallsLine()
{
}

void KLines::generateRandomBalls()
{
    score_undo = score;
    for( int i = 0 ; i < BALLSDROP ; i++ )
    {
      nextBalls_undo[i] = nextBalls[i];    
      nextBalls[i] = bUndo ?
            lsb->random(NCOLORS) :
            nextBalls_redo[i];
    }
    lPrompt->SetBalls(nextBalls);
}

void KLines::placeBalls()
{
    lsb->placeBalls(nextBalls);
}

void KLines::undo()
{
    if (lsb->gameOver())
       return;
    if (!bUndo)
      return;
    for( int i = 0 ; i < BALLSDROP ; i++ )
    {
      nextBalls_redo[i] = nextBalls[i];    
      nextBalls[i] = nextBalls_undo[i];
    }
    score = score_undo;
    updateStat();
    lPrompt->SetBalls(nextBalls);
    lsb->undo();
    switchUndo(FALSE);
}

void KLines::makeTurn()
{
    if (lsb->gameOver())
       return;
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
    statusBar()->changeItem(i18n(" Score: %1").arg(score), 0);
}

void KLines::viewHighScore()
{
    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
    d.exec();
}

void KLines::endGame()
{
    lsb->setGameOver(true);
    lsb->repaint(false);

    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
    KScoreDialog::FieldInfo scoreInfo;
    if (d.addScore(score, scoreInfo, true))
        d.exec();
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

void KLines::keyPressEvent(QKeyEvent *e)
{
    if (lsb->gameOver() && (e->key() == Qt::Key_Space))
    {
        e->accept();
        startGame();
        return;
    }
    KMainWindow::keyPressEvent(e);
}
