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
#include "prefs.h"
#include <kstatusbar.h>
#include "klines.moc"

enum { Nb_Levels = 5 };
static const char *LEVEL[Nb_Levels] = {
    I18N_NOOP("Very Easy"), I18N_NOOP("Easy"), I18N_NOOP("Normal"), I18N_NOOP("Hard"),
    I18N_NOOP("Very Hard")
};


/*
   Creates the KLines widget and sets saved options (if any).
*/

KLines::KLines()
{
  mwidget = new MainWidget(this);
  setCentralWidget( mwidget );

  lsb = mwidget->GetLsb();
  connect(lsb, SIGNAL(endTurn()), this, SLOT(makeTurn()));
  connect(lsb, SIGNAL(eraseLine(int)), this, SLOT(addScore(int)));
  connect(lsb, SIGNAL(endGame()), this, SLOT(endGame()));
  connect(lsb, SIGNAL(userTurn()), this, SLOT(userTurn()));

  lPrompt = mwidget->GetPrompt();

  score = 0;
  score_undo = 0;
  bDemo = false;

  statusBar()->insertItem(i18n(" Score:"), 1, 1);
  statusBar()->setItemAlignment(1, AlignVCenter | AlignLeft);
  statusBar()->insertItem(i18n(" Level: "), 0, 1);
  statusBar()->setItemAlignment(0, AlignVCenter | AlignLeft);
 
  initKAction();

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
  Prefs::setLevel(levelAction->currentItem()-2);
  Prefs::writeConfig();
}

/*
   Init KAction objects (menubar, toolbar, shortcuts)
*/
void KLines::initKAction()
{
  KStdGameAction::gameNew(this, SLOT(startGame()), actionCollection());
  act_demo = KStdGameAction::demo(this, SLOT(startDemo()), actionCollection());
  act_demo->setText(i18n("Start &Tutorial"));
  KStdGameAction::highscores(this, SLOT(viewHighScore()), actionCollection());
  KStdGameAction::quit(this, SLOT(close()), actionCollection());
  endTurnAction = KStdGameAction::endTurn(this, SLOT(makeTurn()), actionCollection());
  showNextAction = new KToggleAction(i18n("&Show Next"), KShortcut(CTRL+Key_P),
                                this, SLOT(switchPrompt()), actionCollection(), "options_show_next");
  showNextAction->setCheckedState(i18n("Hide Next"));
  showNumberedAction = new KToggleAction(i18n("&Use Numbered Balls"), KShortcut(),
                                this, SLOT(switchNumbered()), actionCollection(), "options_show_numbered");
  undoAction = KStdGameAction::undo(this, SLOT(undo()), actionCollection());

  levelAction = KStdGameAction::chooseGameType(0, 0, actionCollection());
  QStringList items;
  for (uint i=0; i<Nb_Levels; i++)
      items.append( i18n(LEVEL[i]) );
  levelAction->setItems(items);

  levelAction->setCurrentItem(Prefs::level()+2);
  showNextAction->setChecked(Prefs::showNext());
  showNumberedAction->setChecked(Prefs::numberedBalls());
  lPrompt->setPrompt(Prefs::showNext());

  (void)new KAction(i18n("Move Left"), Key_Left, lsb, SLOT(moveLeft()), actionCollection(), "left");
  (void)new KAction(i18n("Move Right"), Key_Right, lsb, SLOT(moveRight()), actionCollection(), "right");
  (void)new KAction(i18n("Move Up"), Key_Up, lsb, SLOT(moveUp()), actionCollection(), "up");
  (void)new KAction(i18n("Move Down"), Key_Down, lsb, SLOT(moveDown()), actionCollection(), "down");
  (void)new KAction(i18n("Move Ball"), Key_Space, lsb, SLOT(placePlayerBall()), actionCollection(), "place_ball");

  setupGUI( KMainWindow::Save | Keys | StatusBar | Create );
}

void KLines::startGame()
{
    score = 0;
    score_undo = 0;
    bUndo = true;
    bNewTurn = true;
    if(bDemo)
        stopDemo();

    bFirst = true;

    int level = levelAction->currentItem()-2;
    setLevel(level);

    lsb->setLevel(level);
    lsb->setGameOver(false);
    lsb->clearField();
    generateRandomBalls();
    placeBalls();
    generateRandomBalls();
    undoAction->setEnabled(false);
    endTurnAction->setEnabled(true);
    updateStat();
}

void KLines::setLevel(int level) {
    levelStr = i18n(LEVEL[level+2]);
    statusBar()->changeItem(i18n(" Level: %1").arg(levelStr), 0);
}

void KLines::startDemo()
{
    if (bDemo)
    {
       stopDemo();
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
    undoAction->setEnabled(false);
    endTurnAction->setEnabled(false);
    generateRandomBalls();

    demoStep = 0;
    demoTimer.start(1000, true);
}

void KLines::stopDemo()
{
    bDemo = false;
    lsb->hideDemoText();
    demoTimer.stop();
    statusBar()->changeItem(i18n(" Level: %1").arg(i18n("Tutorial - Stopped")), 0);
    act_demo->setText(i18n("Start &Tutorial"));
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
      statusBar()->changeItem(i18n(" Level: %1").arg(i18n("Tutorial - Paused")), 0);
   }
   KMainWindow::focusOutEvent(ev);
}

void KLines::focusInEvent(QFocusEvent *ev)
{
   if (bDemo)
   {
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
    if (!bDemo){
       placeBalls();
       if(sender() != lsb)
         lsb->saveUndo();
    }
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
    Prefs::setShowNext(!Prefs::showNext());
    lPrompt->setPrompt(Prefs::showNext());
    showNextAction->setChecked(Prefs::showNext());
}

void KLines::switchNumbered()
{
    Prefs::setNumberedBalls(!Prefs::numberedBalls());
    lPrompt->setPrompt(Prefs::showNext());
    showNumberedAction->setChecked(Prefs::numberedBalls());
    mwidget->updatePix();
}

void KLines::switchUndo(bool bu)
{
    bUndo = bu;
    undoAction->setEnabled(bu);
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
