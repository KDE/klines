/*
    SPDX-FileCopyrightText: 2000 Roman Merzlyakov <roman@sbrf.barrt.ru>
    SPDX-FileCopyrightText: 2000 Roman Razilov <roman@sbrf.barrt.ru>
    SPDX-FileCopyrightText: 2007 Dimitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "klines.h"
#include "renderer.h"
#include "prefs.h"
#include "mwidget.h"
#include "scene.h"

#include <QStatusBar>
#include <QAction>

#include <KConfig>
#include <KActionCollection>
#include <KStandardAction>
#include <KToggleAction>
#include <KLocalizedString>

#include <KGameHighScoreDialog>
#include <KGameStandardAction>
#include <KGameThemeSelector>
#include <KGameRenderer>

KLinesMainWindow::KLinesMainWindow()
{
    KLinesRenderer::Init();

    mwidget = new MainWidget(this);
    setCentralWidget( mwidget );

    connect(mwidget->scene(), &KLinesScene::scoreChanged, this, &KLinesMainWindow::updateScore);
    connect(mwidget->scene(), &KLinesScene::stateChanged, this,
            qOverload<const QString &>(&KLinesMainWindow::slotStateChanged));
    connect(mwidget->scene(), &KLinesScene::gameOver, this, &KLinesMainWindow::gameOver);

    scoreLabel->setText(i18n("Score:"));
    statusBar()->addPermanentWidget(scoreLabel);
    
    updateScore(0);

    KGameThemeProvider* prov = KLinesRenderer::renderer()->themeProvider();
    connect(prov, &KGameThemeProvider::currentThemeChanged, this, &KLinesMainWindow::loadSettings);
    mselector = new KGameThemeSelector(KLinesRenderer::renderer()->themeProvider());

    setupActions();

    stateChanged(QStringLiteral( "init" ));
}

KLinesMainWindow::~KLinesMainWindow()
{
  KLinesRenderer::UnInit();
}

void KLinesMainWindow::setupActions()
{
  // Game
  KGameStandardAction::gameNew(this, &KLinesMainWindow::startGame, actionCollection());
  KGameStandardAction::highscores(this, &KLinesMainWindow::viewHighScore, actionCollection());
  KGameStandardAction::quit(this, &QWidget::close, actionCollection());
  KGameStandardAction::end(mwidget->scene(), &KLinesScene::endGame, actionCollection());

  // Move
  KGameStandardAction::undo(mwidget->scene(), &KLinesScene::undo, actionCollection());
  KGameStandardAction::endTurn(mwidget->scene(), &KLinesScene::endTurn, actionCollection());

  // Preferences
  KToggleAction *showNext = actionCollection()->add<KToggleAction>(QStringLiteral( "show_next" ));
  showNext->setText( i18nc("@option:check",  "Show Next" ) );
  connect(showNext, &KToggleAction::triggered, this, &KLinesMainWindow::showNextToggled);

  showNext->setChecked(Prefs::showNext());
  mwidget->setShowNextColors(Prefs::showNext());

  // Navigation
  auto *naviLeft = new QAction( i18nc("@action", "Move Left" ), this );
  naviLeft->setIcon(QIcon::fromTheme(QStringLiteral( "arrow-left"))); 
  KActionCollection::setDefaultShortcut(naviLeft, Qt::Key_Left);
  actionCollection()->addAction( QStringLiteral( "navi_left" ), naviLeft);

  auto *naviRight = new QAction( i18nc("@action", "Move Right" ), this );
  naviRight->setIcon(QIcon::fromTheme(QStringLiteral( "arrow-right")));
  KActionCollection::setDefaultShortcut(naviRight, Qt::Key_Right);
  actionCollection()->addAction( QStringLiteral( "navi_right" ), naviRight);

  auto *naviUp = new QAction( i18nc("@action", "Move Up" ), this );
  naviUp->setIcon(QIcon::fromTheme(QStringLiteral( "arrow-up")));
  KActionCollection::setDefaultShortcut(naviUp, Qt::Key_Up);
  actionCollection()->addAction( QStringLiteral( "navi_up" ), naviUp);

  auto *naviDown = new QAction( i18nc("@action", "Move Down" ), this );
  naviDown->setIcon(QIcon::fromTheme(QStringLiteral( "arrow-down")));
  KActionCollection::setDefaultShortcut(naviDown, Qt::Key_Down);
  actionCollection()->addAction( QStringLiteral( "navi_down" ), naviDown);

  auto *naviSelect = new QAction( i18nc("@action", "Select"), this );
  KActionCollection::setDefaultShortcut(naviSelect, Qt::Key_Space);
  actionCollection()->addAction( QStringLiteral( "navi_select" ), naviSelect);

  connect( naviLeft, &QAction::triggered, mwidget->scene(), &KLinesScene::moveFocusLeft);
  connect( naviRight, &QAction::triggered, mwidget->scene(), &KLinesScene::moveFocusRight);
  connect( naviUp, &QAction::triggered, mwidget->scene(), &KLinesScene::moveFocusUp);
  connect( naviDown, &QAction::triggered, mwidget->scene(), &KLinesScene::moveFocusDown);
  connect( naviSelect, &QAction::triggered, mwidget->scene(), &KLinesScene::cellSelected);

  KStandardAction::preferences(mselector, [this]() { mselector->showAsDialog(); }, actionCollection());
  setupGUI();
}

void KLinesMainWindow::updateScore(int score)
{
    scoreLabel->setText(i18n("Score: %1", score));
}

void KLinesMainWindow::gameOver(int score)
{
    KGameHighScoreDialog d(KGameHighScoreDialog::Name | KGameHighScoreDialog::Score, this);
    d.setConfigGroup(qMakePair(QByteArray("Highscore"), i18n("High Scores")));
    d.addScore(score, KGameHighScoreDialog::AskName);
    d.exec();
}

void KLinesMainWindow::viewHighScore()
{
   KGameHighScoreDialog d(KGameHighScoreDialog::Name | KGameHighScoreDialog::Score, this);
   d.setConfigGroup(qMakePair(QByteArray("Highscore"), i18n("High Scores")));
   d.exec();
}

void KLinesMainWindow::startGame()
{
    updateScore(0);
    mwidget->scene()->startNewGame();
    mwidget->setShowNextColors(Prefs::showNext());
}

void KLinesMainWindow::showNextToggled(bool show)
{
    mwidget->setShowNextColors(show);
    Prefs::setShowNext(show);
    Prefs::self()->save();
}


// FIXME these are strings from old tutorial
// leave them if I ever want to use them when I'll impelement tutorial mode
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

void KLinesMainWindow::loadSettings()
{
    KLinesRenderer::loadTheme();
    QRectF r = mwidget->scene()->sceneRect();
    mwidget->scene()->invalidate( r, QGraphicsScene::BackgroundLayer ); // redraw background
    mwidget->scene()->resizeScene( (int)r.width(), (int)r.height() ); // redraw scene
}

#include "moc_klines.cpp"
