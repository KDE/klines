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

KLines::KLines() : KMainWindow(0, "klines", WType_TopLevel)
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

  connect(&demoTimer, SIGNAL(timeout()), this, SLOT(slotDemo()));

  setFocusPolicy(StrongFocus);
  setFocus();

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
  config->writeEntry("Level", level);
  bool show_next = lPrompt->getState();
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
  KStdGameAction::gameNew(this, SLOT(startGame()), actionCollection());
  act_demo = new KAction(i18n("Start &Tutorial"), KShortcut(CTRL+Key_D), this, SLOT(startDemo()), actionCollection(), "game_demo");
  KStdGameAction::highscores(this, SLOT(viewHighScore()), actionCollection());
  KStdGameAction::quit(kapp, SLOT(quit()), actionCollection());
  (void)new KAction(i18n("Ne&xt Turn"), Key_N, this, SLOT(makeTurn()), actionCollection(), "next_turn");
  KToggleAction* a = new KToggleAction(i18n("&Show Next"), KShortcut(CTRL+Key_P), this, SLOT(switchPrompt()), actionCollection(), "show_next");
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
    bDemo = false;
    act_demo->setText(i18n("Start &Tutorial"));
    bFirst = true;

    int level = currentLevel(&levelStr);
    statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);

    lsb->setLevel(level);
    lsb->setGameOver(false);
    lsb->clearField();
    generateRandomBalls();
    placeBalls();
    generateRandomBalls();
    actionCollection()->action("edit_undo")->setEnabled(false);
    actionCollection()->action("next_turn")->setEnabled(true);
    updateStat();
}

void KLines::startDemo()
{
    if (bDemo)
    {
       bDemo = false;
       lsb->hideDemoText();
       demoTimer.stop();
       levelStr = i18n("Tutorial - Stopped");
       statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);
       act_demo->setText(i18n("Start &Tutorial"));
       return;
    }
    score = 0;
    score_undo = 0;
    bUndo = true;
    bNewTurn = true;
    bDemo = true;
    act_demo->setText(i18n("Stop &Tutorial"));
    bFirst = true;

    levelStr = i18n("Tutorial");
    statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);

    lsb->startDemoMode();
    lsb->setGameOver(false);
    lsb->clearField();
    lsb->update();
    actionCollection()->action("edit_undo")->setEnabled(false);
    actionCollection()->action("next_turn")->setEnabled(false);
    generateRandomBalls();
  
    demoStep = 0;
    demoTimer.start(1000, true);
}

void KLines::slotDemo()
{
    bool newBalls = false;
    int ballColors = -1;
    int clickX = 0;
    int clickY = 0;
    QString msg;
    demoStep++;
    if ((demoStep % 2) == 0)
    {
       lsb->hideDemoText();
       demoTimer.start(1000, true);
       return;
    }
    if (demoStep == 1)
    {
       msg = i18n("The goal of the game is to put\n"
                  "5 balls of the same color in line.");
    }
    else if (demoStep == 3)
    {
       newBalls = true;
    }
    else if (demoStep == 5)
    {
       msg = i18n("You can make horizontal, vertical\n"
                  "and diagonal lines.");
    }
    else if (demoStep == 7)
    {
       newBalls = true;
    }
    else if (demoStep == 9)
    {
       msg = i18n("Each turn, three new balls are placed on the board.");
    }
    else if (demoStep == 11)
    {
       newBalls = true;
    }
    else if (demoStep == 13)
    {
       msg = i18n("Every turn, you can move one ball.");
    }
    else if (demoStep == 15)
    {
       newBalls = true;
       ballColors = 56;
    }
    else if (demoStep == 17)
    {
       msg = i18n("To move a ball, click on it with the mouse,\n"
                  "then click where you want the ball to go.");
    }
    else if (demoStep == 19)
    {
       clickX = 6;
       clickY = 6;
    }
    else if (demoStep == 21)
    {
       clickX = 6;
       clickY = 9;
    }
    else if (demoStep == 23)
    {
       msg = i18n("You just moved the blue ball!");
    }
    else if (demoStep == 25)
    {
       newBalls = true;
    }
    else if (demoStep == 27)
    {
       msg = i18n("Balls can be moved to every position on the board,\n"
                  "as long as there are no other balls in their way.");
    }
    else if (demoStep == 29)
    {
       clickX = 4;
       clickY = 3;
       demoStep++;
    }
    else if (demoStep == 31)
    {
       clickX = 7;
       clickY = 9;
    }
    else if (demoStep == 33)
    {
       msg = i18n("Now we only need one more blue ball.");
    }
    else if (demoStep == 35)
    {
       newBalls = true;
    }
    else if (demoStep == 37)
    {
       msg = i18n("It seems to be our lucky day!");
    }
    else if (demoStep == 39)
    {
       clickX = 8;
       clickY = 2;
       demoStep++;
    }
    else if (demoStep == 41)
    {
       clickX = 8;
       clickY = 9;
    }
    else if (demoStep == 43)
    {
       msg = i18n("Hurray! And away they go!\n"
                  "Now lets try the green balls.");
    }
    else if (demoStep == 45)
    {
       clickX = 8;
       clickY = 7;
       demoStep++;
    }
    else if (demoStep == 47)
    {
       clickX = 4;
       clickY = 5;
       lsb->demoAdjust(42);
    }
    else if (demoStep == 49)
    {
       newBalls = true;
    }
    else if (demoStep == 51)
    {
       msg = i18n("Now you try!\n"
                  "Click on the green ball and move it to the others!");
       demoStep++;
    }
    else if (demoStep == 53)
    {
       lsb->hideDemoText();
       lsb->adjustDemoMode(true, false);
       demoStep++;
    }
    else if (demoStep == 55)
    {
       msg = i18n("Almost, try again!");
       demoStep -= 4;
    }
    else if (demoStep == 57)
    {
       msg = i18n("Very good!");
    }
    else if (demoStep == 59)
    {
       msg = i18n("Whenever you complete a line you get an extra turn.");
    }
    else if (demoStep == 61)
    {
       msg = i18n("This is the end of this tutorial.\n"
                  "Feel free to finish the game!");
       demoStep++;
    }
    else if (demoStep == 63)
    {
       lsb->hideDemoText();
       lsb->adjustDemoMode(true, true);
       bDemo = false;
       act_demo->setText(i18n("Start &Tutorial"));
    }

    if (!msg.isEmpty())
    {
       lsb->showDemoText(msg);
       demoTimer.start(3500 + msg.contains("\n")*1500, true);
       return;
    }
    if (newBalls)
    {
       placeBalls();
       if (ballColors == -1)
       {
          generateRandomBalls();
       }
       else
       {
          for( int i = 0 ; i < BALLSDROP ; i++ )
          {
             nextBalls[i] = ballColors % 10;
             ballColors = ballColors / 10;
             lPrompt->SetBalls(nextBalls);
          }
       }
          
       updateStat();
       demoTimer.start(1000, true);
       return;
    }
    if (clickX)
    {
       lsb->demoClick(clickX-1, clickY-1);
       if (hasFocus())
          demoTimer.start(1000, true);
       return;
    }
}

void KLines::focusOutEvent(QFocusEvent *ev)
{
   if (bDemo)
   {
      lsb->hideDemoText();
      demoTimer.stop();
      levelStr = i18n("Tutorial - Paused");
      statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);
   }
   KMainWindow::focusOutEvent(ev);
}

void KLines::focusInEvent(QFocusEvent *ev)
{
   if (bDemo)
   {
      levelStr = i18n("Tutorial");
      statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);
      slotDemo();
   }
   KMainWindow::focusInEvent(ev);
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
    if (bDemo)
    {
       lsb->adjustDemoMode(false, false);
       demoTimer.start(100, true);
    }
    if (lsb->gameOver())
       return;
    if (!bDemo)
       placeBalls();
    bNewTurn = true;
}

void KLines::userTurn()
{
    if(bNewTurn)
    {
       bNewTurn = false;
       if (!bDemo)
          generateRandomBalls();
       if (!bFirst && !bDemo)
          switchUndo(true);
    }
    bFirst = false;
}

void KLines::addScore(int ballsErased)
{   if(ballsErased >= 5){
      score += 2*ballsErased*ballsErased - 20*ballsErased + 60 ;
      if( !lPrompt->getState() ) score+= 1;
      updateStat();
    };
    if (bDemo)
    {
      lsb->adjustDemoMode(false, false);
      demoStep += 2;
      if (hasFocus())
         demoTimer.start(100, true);
    }
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

    if (bDemo)
       return;
       
    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo.insert(KScoreDialog::Level, levelStr);
    if (d.addScore(score, scoreInfo, true))
        d.exec();
}

void KLines::switchPrompt()
{
    lPrompt->setPrompt(!lPrompt->getState());
    ((KToggleAction*)actionCollection()->action("show_next"))->setChecked(lPrompt->getState());
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
