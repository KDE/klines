/***************************************************************************
                          highscore.h  -  description
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
#include <kapp.h>


const int HIGHSCORE_MAX = 10;

struct HighScore {
    int  score;
    QString name;

	  HighScore& operator= ( HighScore& hs )
	  {
			score = hs.score;
			name = hs.name;
			return *this;
		};
		
		HighScore( ) : score(0),name("")
    {
		};

		HighScore( HighScore& hs )
    {
			score = hs.score;
			name = hs.name;
		};
};


class HScore: public QObject {
  Q_OBJECT

private:

  HighScore highscore[HIGHSCORE_MAX];
  int hiscore_used;

  QString getPlayerName();
  QString playername;

  void readHighscore(KConfig *cfg);
  void readHighscore();
  void writeHighscore();
  int  insertHighscore(HighScore &);
  bool isBetter(HighScore &, HighScore &);
  void showHighscore(int focusitem = -1);

public:
  HScore();
  ~HScore();
  int getMaxScore();

public slots:
  void slotEndOfGame(int score);
  void viewHighScore();

};


