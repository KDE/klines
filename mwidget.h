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

#ifndef MWIDGET_H
#define MWIDGET_H

#include <QFrame>

class BallPainter;
class LinesBoard;
class LinesPrompt;

class KLinesScene;
class BallsPreview;

class MainWidget : public QFrame
{
    Q_OBJECT
public:
  MainWidget( QWidget* parent=0 );
  ~MainWidget();
  LinesBoard * GetLsb();
  LinesPrompt * GetPrompt();
  void updatePix();

  KLinesScene* scene() { return m_scene; }
private slots:
    void updateNextColors();
private:
  LinesBoard * lsb;
  LinesPrompt * lPrompt;
  BallPainter *bPainter;

  KLinesScene* m_scene;
  BallsPreview* m_preview;
};

#endif
