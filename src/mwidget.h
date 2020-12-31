/*
    SPDX-FileCopyrightText: 2000 Roman Merzlyakov <roman@sbrf.barrt.ru>
    SPDX-FileCopyrightText: 2000 Roman Razilov <Roman.Razilov@gmx.de>
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MWIDGET_H
#define MWIDGET_H

#include <QWidget>

class KLinesScene;
class QLabel;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

    KLinesScene* scene() { return m_scene; }
public Q_SLOTS:
    void setShowNextColors(bool);
private:
    void resizeEvent( QResizeEvent* ) override;
    KLinesScene* m_scene;
    QLabel *m_next_label;
};

#endif
