/***************************************************************************
                          klines.h  -  description
                             -------------------
    begin                : Fri May 19 2000
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
#ifndef KLINES_H
#define KLINES_H

#include <qwidget.h>
#include <qmenubar.h> 
#include <qlabel.h>
#include <qpushbutton.h>

#include <kmainwindow.h>

#include "linesboard.h"
#include "mwidget.h"
#include "prompt.h"

class KRadioAction;

class KLines : public KMainWindow
{
  Q_OBJECT
public:
  KLines();
  ~KLines();

protected:
  void keyPressEvent(QKeyEvent *e);
  void initKAction();
  void setLevel(int level);
  int currentLevel(QString *levelStr=0);

public slots:
  void startGame();
  void stopGame();
  void endGame();
  void makeTurn();
  void userTurn();
  void addScore(int ballsErased);
  void switchPrompt();
  void undo();
  
private slots:
  void viewHighScore();

private:
  LinesBoard* lsb;
  MainWidget *mwidget;
  LinesPrompt *lPrompt;
  KRadioAction *act_level1;
  KRadioAction *act_level2;
  KRadioAction *act_level3;
  KRadioAction *act_level4;
  KRadioAction *act_level5;
  QString levelStr;

  bool bNewTurn;

  int score;
  int score_undo;

  int nextBalls[BALLSDROP];
  int nextBalls_undo[BALLSDROP];
  int nextBalls_redo[BALLSDROP];
  bool bUndo;

  void searchBallsLine();
  void generateRandomBalls();
  void placeBalls();
  void updateStat();
  void switchUndo( bool bu );
};

#endif
