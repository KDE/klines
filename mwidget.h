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

#ifndef MWIDGET_H
#define MWIDGET_H

#include <QWidget>

class KLinesScene;
class QLabel;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget( QWidget* parent=0 );
    ~MainWidget();

    KLinesScene* scene() { return m_scene; }
public slots:
    void setShowNextColors(bool);
private:
    virtual void resizeEvent( QResizeEvent* );
    KLinesScene* m_scene;
    QLabel *m_next_label;
};

#endif
