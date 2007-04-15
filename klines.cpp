/***************************************************************************
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
    copyright            : (C) 2006 by Dmitry Suzdalev
    email                : dimsuz@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "klines.h"
#include "prefs.h"
#include "mwidget.h"
#include "scene.h"

#include <KConfig>
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KToggleAction>
#include <KStatusBar>
#include <KLocale>

#include <kscoredialog.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <kstandardgameaction.h>

KLinesMainWindow::KLinesMainWindow()
{
  mwidget = new MainWidget(this);
  setCentralWidget( mwidget );

  connect(mwidget->scene(), SIGNAL(scoreChanged(int)), SLOT(updateScore(int)));
  connect(mwidget->scene(), SIGNAL(gameOver(int)), SLOT(gameOver(int)));

  statusBar()->insertItem(i18n("Score:"), 0);
  updateScore(0);

  initKAction();
}

KLinesMainWindow::~KLinesMainWindow()
{
}

void KLinesMainWindow::initKAction()
{
  QAction *action;

  action = KStandardGameAction::gameNew(this, SLOT(startGame()), this);
  actionCollection()->addAction(action->objectName(), action);

  action = KStandardGameAction::highscores(this, SLOT(viewHighScore()), this);
  actionCollection()->addAction(action->objectName(), action);
  action = KStandardGameAction::quit(this, SLOT(close()), this);
  actionCollection()->addAction(action->objectName(), action);
  action = KStandardGameAction::endTurn(mwidget->scene(), SLOT(endTurn()), this);
  actionCollection()->addAction(action->objectName(), action);

  action = KStandardGameAction::undo(mwidget->scene(), SLOT(undo()), this);
  actionCollection()->addAction(action->objectName(), action);
  action->setEnabled(false); 	 
  connect( mwidget->scene(), SIGNAL(enableUndo(bool)), action, SLOT(setEnabled(bool)) );

  KToggleAction *showNext = actionCollection()->add<KToggleAction>("show_next");
  showNext->setText(i18n("&Show Next"));
  showNext->setShortcut(KShortcut(Qt::CTRL+Qt::Key_P));
  connect(showNext, SIGNAL(triggered(bool) ), SLOT(showNextToggled(bool)));
  addAction(showNext);

  showNext->setChecked(Prefs::showNext());
  mwidget->setShowNextColors(Prefs::showNext());

  action = actionCollection()->addAction("left");
  action->setText(i18n("Move Left"));
  connect(action, SIGNAL(triggered(bool) ), mwidget->scene(), SLOT(moveFocusLeft()));
  action->setShortcut(Qt::Key_Left);
  addAction(action);

  action = actionCollection()->addAction("right");
  action->setText(i18n("Move Right"));
  connect(action, SIGNAL(triggered(bool) ), mwidget->scene(), SLOT(moveFocusRight()));
  action->setShortcut(Qt::Key_Right);
  addAction(action);

  action = actionCollection()->addAction("up");
  action->setText(i18n("Move Up"));
  connect(action, SIGNAL(triggered(bool) ), mwidget->scene(), SLOT(moveFocusUp()));
  action->setShortcut(Qt::Key_Up);
  addAction(action);

  action = actionCollection()->addAction("down");
  action->setText(i18n("Move Down"));
  connect(action, SIGNAL(triggered(bool) ), mwidget->scene(), SLOT(moveFocusDown()));
  action->setShortcut(Qt::Key_Down);
  addAction(action);

  action = actionCollection()->addAction("select_cell");
  action->setText(i18n("Move Ball"));
  connect(action, SIGNAL(triggered(bool) ), mwidget->scene(), SLOT(cellSelected()));
  action->setShortcut(Qt::Key_Space);
  addAction(action);

  setupGUI( Save | Keys | StatusBar | Create );
}

void KLinesMainWindow::updateScore(int score)
{
    statusBar()->changeItem(i18n("Score: %1", score), 0);
}

void KLinesMainWindow::gameOver(int score)
{
    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
    KScoreDialog::FieldInfo scoreInfo;
    d.addScore(score, scoreInfo, KScoreDialog::AskName);
    d.exec();
}

void KLinesMainWindow::viewHighScore()
{
   KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
   d.exec();
}

void KLinesMainWindow::startGame()
{
    updateScore(0);
    mwidget->scene()->startNewGame();
    mwidget->updateNextColors();
}

void KLinesMainWindow::showNextToggled(bool show)
{
    mwidget->setShowNextColors(show);
    Prefs::setShowNext(show);
    Prefs::writeConfig();
}


// FIXME these are strings from old tutorial
// leave them if I want to use them when I'll impelement tutorial mode
/**
       msg = i18n("The goal of the game is to put\n"
       msg = i18n("You can make horizontal, vertical\n"
                  "and diagonal lines.");
       msg = i18n("Each turn, three new balls are placed on the board.");
       msg = i18n("Every turn, you can move one ball.");
       msg = i18n("To move a ball, click on it with the mouse,\n"
                  "then click where you want the ball to go.");
       msg = i18n("You just moved the blue ball!");
       msg = i18n("Balls can be moved to every position on the board,\n"
                  "as long as there are no other balls in their way.");
       msg = i18n("Now we only need one more blue ball.");
       msg = i18n("It seems to be our lucky day!");
       msg = i18n("Hurray! And away they go!\n"
                  "Now lets try the green balls.");
       msg = i18n("Now you try!\n"
                  "Click on the green ball and move it to the others!");
       msg = i18n("Almost, try again!");
       msg = i18n("Very good!");
       msg = i18n("Whenever you complete a line you get an extra turn.");
       msg = i18n("This is the end of this tutorial.\n"
                  "Feel free to finish the game!");
                  */

#include "klines.moc"
