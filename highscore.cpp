/***************************************************************************
                          highscore.cpp  -  description
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
#include <qlineedit.h>
#include <qlayout.h>
#include <qfont.h>
#include <kmessagebox.h>
#include <kseparator.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <kconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include "highscore.h"
#include <klocale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>

HScore::HScore():QObject() {
  readHighscore();
}

HScore::~HScore() {
}

void HScore::slotEndOfGame(int score) {

    // create highscore entry
    HighScore hs;
    hs.score = score;

    // check if we made it into Top10
    bool isHighscore = FALSE;
    if(hiscore_used < HIGHSCORE_MAX)
      isHighscore = TRUE;
    else if(isBetter(hs, highscore[HIGHSCORE_MAX-1]))
      isHighscore = TRUE;

    if(isHighscore) {
      hs.name = getPlayerName();
      int rank = insertHighscore(hs);
			writeHighscore();
      showHighscore(rank);
    } else {
      QString s;
      s.sprintf( i18n("Congratulations! You score is %i"), score );

      KMessageBox::information(NULL, s.data(), i18n("Game over"));
//      KMsgBox::message(this, locale->translate("End of game"), s.data());
    }
}

QString HScore::getPlayerName() {
  QDialog *dlg = new QDialog(NULL, i18n("Hall Of Fame"), TRUE);

  QLabel  *l1  = new QLabel(i18n("You've made in into the \"Hall Of Fame\".Type in\nyour name so mankind will always remember\nyour cool rating."), dlg);
//  QLabel  *l1  = new QLabel(locale->translate("You've made in into the \"Hall Of Fame\".Type in\nyour name so mankind will always remember\nyour cool rating."), dlg);
  l1->setFixedSize(l1->sizeHint());

  QLabel *l2 = new QLabel(i18n("Your name:"), dlg);
//  QLabel *l2 = new QLabel(locale->translate("Your name:"), dlg);
  l2->setFixedSize(l2->sizeHint());

  QLineEdit *e = new QLineEdit(dlg);
  e->setText("XXXXXXXXXXXXXXXX");
  e->setMinimumWidth(e->sizeHint().width());
  e->setFixedHeight(e->sizeHint().height());
  e->setText(playername);
  e->setFocus();

  QPushButton *b = new QPushButton( i18n("OK"), dlg);
  b->setDefault(TRUE);
//  if(style() == MotifStyle)
//    b->setFixedSize(b->sizeHint().width() + 10,
//		    b->sizeHint().height() +10);
//  else
  b->setFixedSize(b->sizeHint());
  connect(b, SIGNAL(released()), dlg, SLOT(accept()));
  connect(e, SIGNAL(returnPressed()),
	  dlg, SLOT(accept()));

  // create layout
  QVBoxLayout *tl = new QVBoxLayout(dlg, 10);
  QHBoxLayout *tl1 = new QHBoxLayout();
  tl->addWidget(l1);
  tl->addSpacing(5);
  tl->addLayout(tl1);
  tl1->addWidget(l2);
  tl1->addWidget(e);
  tl->addSpacing(5);
  tl->addWidget(b);
  tl->activate();
  tl->freeze();

  dlg->exec();

  playername = e->text();
  delete dlg;
  QString stripped=playername.stripWhiteSpace();

  if (stripped.isEmpty())
    playername=i18n("Anonymous");
  else playername=stripped;

  return playername;
}

bool HScore::isBetter(HighScore &hs, HighScore &than) {
  if(hs.score > than.score) return TRUE;
  else return FALSE;
}

int HScore::insertHighscore(HighScore& hs) {
  int i;

  for (i=hiscore_used;i>0;i--) {
	  if (hs.score<=highscore[i-1].score) break;
	  if (i<HIGHSCORE_MAX) {
	       highscore[i]=highscore[i-1];
	  }
  }
  if (i<HIGHSCORE_MAX) {
	  highscore[i] = hs;
	  if (hiscore_used<HIGHSCORE_MAX) hiscore_used++;
	  writeHighscore();
    return i;
  }
	return -1;
}

void HScore::readHighscore()
{
    QString file = locateLocal("appdata", "highscore");
    if (KStandardDirs::exists(file)) {
        KSimpleConfig config(file);
        readHighscore(&config);
    } else {
        readHighscore(KGlobal::config());
    }
}

void HScore::readHighscore(KConfig *cfg) {

  KConfigGroupSaver cs(cfg, "Highscore");

  hiscore_used=0;
  for (int i=0;i<HIGHSCORE_MAX;i++) {
      QString name;
      name.sprintf("Rank_%i",i);
      if (!cfg->hasKey(name)) break;
      // kdDebug() << "load " << i << endl;
      QStringList value = cfg->readListEntry(name, ' ');
      highscore[i].score = (*value.at(0)).toInt();
      highscore[i].name = (*value.at(1));;
      hiscore_used++;
  }
  playername=cfg->readEntry("LastPlayer",i18n("Anonymous"));

}


void HScore::writeHighscore() {

  QString file = locateLocal("appdata", "highscore");
  struct stat sst;
  mode_t mode = S_IRUSR | S_IWUSR;
  if (!::stat(file.local8Bit(), &sst))
      mode = sst.st_mode;

  KSimpleConfig *config = new KSimpleConfig(file);
  config->setGroup("Highscore");

  for (int i=0;i<hiscore_used;i++) {
      QString name = QString::fromLatin1("Rank_%1").arg(i);
      QString value = QString::fromLatin1("%1 %2").
                      arg(highscore[i].score).
                      arg(highscore[i].name);
      config->writeEntry(name, value);
  }
  config->writeEntry("LastPlayer",playername);
  delete config;
  ::chmod(file.local8Bit(), mode);
}

void HScore::showHighscore(int focusitem)  {
  // this may look a little bit confusing...
  QDialog *dlg = new QDialog(0, i18n("Highscore"), TRUE);
//  QDialog *dlg = new QDialog(0, locale->translate("Hall Of Fame"), TRUE);

  dlg->setCaption(i18n("Lines: Highscore"));
//  dlg->setCaption(locale->translate("Shisen-Sho: Hall Of Fame"));

  QVBoxLayout *tl = new QVBoxLayout(dlg, 10);
  
  QLabel *l = new QLabel(i18n("Highscore"), dlg);
//  QLabel *l = new QLabel(locale->translate("Hall Of Fame"), dlg);

  QFont f = dlg->font();
//  QFont f = font();

  f.setPointSize(24);
  f.setBold(TRUE);
  l->setFont(f);
  l->setFixedSize(l->sizeHint());
  l->setFixedWidth(l->width() + 32);
  l->setAlignment(AlignCenter);
  tl->addWidget(l);

  // insert highscores in a gridlayout
  QGridLayout *table = new QGridLayout(12, 3, 5);
  tl->addLayout(table, 1);

  // add a separator line
  KSeparator *sep = new KSeparator(dlg);
  table->addMultiCellWidget(sep, 1, 1, 0, 2);

  // add titles
  f = dlg->font();
  f.setBold(TRUE);

  l = new QLabel(i18n("Rank"), dlg);
//  l = new QLabel(locale->translate("Rank"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 0);

  l = new QLabel(i18n("Name"), dlg);
//  l = new QLabel(locale->translate("Name"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 1);

//  l = new QLabel(locale->translate("Score"), dlg);
  l = new QLabel(i18n("Score"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 2);
  
  QString s;
  QLabel *e[HIGHSCORE_MAX][3];
  int i, j;

  for(i = 0; i < HIGHSCORE_MAX; i++) {
    HighScore hs;
    if(i < hiscore_used )
      hs = highscore[i];   
    
    // insert rank    
    s.sprintf("%d", i+1);
    e[i][0] = new QLabel(s.data(), dlg);

    // insert name
    if(i < hiscore_used)
      e[i][1] = new QLabel(hs.name, dlg);
    else
      e[i][1] = new QLabel("", dlg);

    // insert score
    if(i < hiscore_used) {
      s.sprintf("%i", hs.score);
      e[i][2] = new QLabel(s.data(), dlg);
    } else
      e[i][2] = new QLabel("", dlg);
      e[i][2]->setAlignment(AlignRight);

  }

  f = dlg->font();
  f.setBold(TRUE);
  f.setItalic(TRUE);
  for(i = 0; i < HIGHSCORE_MAX; i++)
    for(j = 0; j < 3; j++) {
      e[i][j]->setMinimumHeight(e[i][j]->sizeHint().height());
      if(j == 1)
	e[i][j]->setMinimumWidth(QMAX(e[i][j]->sizeHint().width(), 100));
      else
	e[i][j]->setMinimumWidth(QMAX(e[i][j]->sizeHint().width(), 60));
      if((int)i == focusitem)
	e[i][j]->setFont(f);
      table->addWidget(e[i][j], i+2, j, AlignCenter);	
    }
    
  QPushButton *b = new QPushButton(i18n("Close"), dlg);
//  QPushButton *b = new QPushButton(locale->translate("Close"), dlg);
//  if(style() == MotifStyle)
//    b->setFixedSize(b->sizeHint().width() + 10,
//		    b->sizeHint().height() + 10);
//  else
  b->setFixedSize(b->sizeHint());

  // connect the "Close"-button to done
  connect(b, SIGNAL(clicked()),
	  dlg, SLOT(accept()));
  b->setDefault(TRUE);
  b->setFocus();

  // make layout
  tl->addSpacing(HIGHSCORE_MAX);
  tl->addWidget(b);
  tl->activate();
  tl->freeze();

  dlg->exec();
  delete dlg;
}


void HScore::viewHighScore()
{
    showHighscore(-1);
}

int HScore::getMaxScore()
{
    int m=0;
    for(int i=0; i<hiscore_used; i++)
      if( highscore[i].score > m ) m = highscore[i].score;

    return m;

}
