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
#include <kaction.h>
#include <kstdaction.h>
#include <kstdgameaction.h>


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
  connect(lsb, SIGNAL(userTurn()), this, SLOT(userTurn()));

  lPrompt = mwidget->GetPrompt();

  initKAction();

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

/*
   Init KAction objects (menubar, toolbar, shortcuts)
*/
void KLines::initKAction()
{
// game
  KStdGameAction::gameNew(this, SLOT(stopGame()), actionCollection());
  KStdGameAction::highscores(this, SLOT(viewHighScore()), actionCollection());
  KStdGameAction::quit(kapp, SLOT(quit()), actionCollection());
  (void)new KAction(i18n("Ne&xt"), Key_N, this, SLOT(makeTurn()), actionCollection(), "game_next");
  KToggleAction* a = new KToggleAction(i18n("&Show Next"), KShortcut(CTRL+Key_P), this, SLOT(switchPrompt()), actionCollection(), "game_show_next");
  a->setChecked(lPrompt->getState());

// edit
  KStdAction::undo(this, SLOT(undo()), actionCollection());

// init using klinesui.rc
  createGUI();
}

void KLines::startGame()
{
    score = 0;
    score_undo = 0;
    bUndo = true;
    bNewTurn = true;

    lsb->setGameOver(false);
    lsb->clearField();
    generateRandomBalls();
    placeBalls();
    generateRandomBalls();
    actionCollection()->action("edit_undo")->setEnabled(false);
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
    bNewTurn = true;
}

void KLines::userTurn()
{
    if(bNewTurn)
    {
       bNewTurn = false;
       generateRandomBalls();
       switchUndo(TRUE);
    }
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
    ((KToggleAction*)actionCollection()->action("game_show_next"))->setChecked(lPrompt->getState());
}

void KLines::switchUndo(bool bu)
{
    bUndo = bu;
    actionCollection()->action("edit_undo")->setEnabled(bu);
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
