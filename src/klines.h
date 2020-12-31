/*
    SPDX-FileCopyrightText: 2000 Roman Razilov <Roman.Razilov@gmx.de>
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KLINES_H
#define KLINES_H

#include <QLabel>
#include <QPointer>

#include <KXmlGuiWindow>

class KgThemeSelector;

class MainWidget;

class KLinesMainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    KLinesMainWindow();
    ~KLinesMainWindow();

protected:
    void setupActions();

public Q_SLOTS:
    void startGame();
private Q_SLOTS:
    void viewHighScore();
    void updateScore(int score);
    void gameOver(int score);
    void showNextToggled(bool show);
    void loadSettings();
private:
    KgThemeSelector *mselector;
    MainWidget *mwidget;
    
    QPointer<QLabel> scoreLabel = new QLabel;

};

#endif
