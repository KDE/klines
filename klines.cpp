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
#include <kconfig.h>
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

  statusBar()->insertItem(i18n(" Score:"), 1, 1);
  statusBar()->setItemAlignment(1, AlignVCenter | AlignLeft);
  statusBar()->insertItem(i18n(" Level: "), 0, 1);
  statusBar()->setItemAlignment(0, AlignVCenter | AlignLeft);

  startGame();
}

/*
   Saves the options and destroys the KLines widget.
*/
KLines::~KLines()
{
  KConfig *config = kapp->config();
  config->setGroup("Game");
  int level = currentLevel();
qWarning("Level = %d", level);
  config->writeEntry("Level", level);
  bool show_next = lPrompt->getState();
qWarning("Prompt = %s", show_next ? "true" : "false");
  config->writeEntry("ShowNext", show_next);
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
  act_level1 = new KRadioAction(i18n("Very Easy"), KShortcut(), actionCollection(), "settings_level1");
  act_level2 = new KRadioAction(i18n("Easy"), KShortcut(), actionCollection(), "settings_level2");
  act_level3 = new KRadioAction(i18n("Normal"), KShortcut(), actionCollection(), "settings_level3");
  act_level4 = new KRadioAction(i18n("Hard"), KShortcut(), actionCollection(), "settings_level4");
  act_level5 = new KRadioAction(i18n("Very Hard"), KShortcut(), actionCollection(), "settings_level5");
  act_level1->setExclusiveGroup("levels"); 
  act_level2->setExclusiveGroup("levels"); 
  act_level3->setExclusiveGroup("levels"); 
  act_level4->setExclusiveGroup("levels"); 
  act_level5->setExclusiveGroup("levels"); 

  KConfig *config = kapp->config();
  config->setGroup("Game");
  int level = config->readNumEntry("Level", 0);
  bool show_next = config->readBoolEntry("ShowNext", true);
  setLevel(level);
  a->setChecked(show_next);
  lPrompt->setPrompt(show_next);
  
// edit
  KStdAction::undo(this, SLOT(undo()), actionCollection());

// init using klinesui.rc
  createGUI();
}



void KLines::setLevel(int level)
{
    switch(level)
    {
       case -1:
       	 act_level2->setChecked(true);
       	 break;
       case 0:
       	 act_level3->setChecked(true);
       	 break;
       case 1:
       	 act_level4->setChecked(true);
       	 break;
       case 2:
       	 act_level5->setChecked(true);
       	 break;
       default:
       	 act_level1->setChecked(true);
       	 break;
    }
}

int KLines::currentLevel(QString *levelStr)
{
    int level;
    if (act_level2->isChecked())
    {
       level = -1;
       if (levelStr)
           *levelStr = act_level2->text();
    }
    else if (act_level3->isChecked())
    {
       level = 0;
       if (levelStr)
          *levelStr = act_level3->text();
    }
    else if (act_level4->isChecked())
    {
       level = 1;
       if (levelStr)
          *levelStr = act_level4->text();
    }
    else if (act_level5->isChecked())
    {
       level = 2;
       if (levelStr)
          *levelStr = act_level5->text();
    }
    else
    {
       level = -2;
       if (levelStr)
          *levelStr = act_level1->text();
    }
    return level;
}

void KLines::startGame()
{
    score = 0;
    score_undo = 0;
    bUndo = true;
    bNewTurn = true;

    int level = currentLevel(&levelStr);
    statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);

    lsb->setLevel(level);
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
    statusBar()->changeItem(i18n(" Score: %1").arg(score), 1);
}

void KLines::viewHighScore()
{
    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);
    d.exec();
}

void KLines::endGame()
{
    lsb->setGameOver(true);
    lsb->repaint(false);

    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo.insert(KScoreDialog::Level, levelStr);
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
