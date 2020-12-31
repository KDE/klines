/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PREVIEWITEM_H
#define PREVIEWITEM_H

#include <QGraphicsItem>
#include "commondefs.h"

class PreviewItem : public QGraphicsItem
{
public:
    explicit PreviewItem( QGraphicsScene* scene );

    /**
     * Sets colors to be displayed
     */
    void setPreviewColors( const QList<BallColor>& colors );

    // reimplemented functions
    void paint( QPainter* p, const QStyleOptionGraphicsItem* option,  QWidget* widget = Q_NULLPTR ) Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
private:
    QList<BallColor> m_colors;
};

#endif
