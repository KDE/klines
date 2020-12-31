/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef COMMONDEFS_H
#define COMMONDEFS_H
#include <QObject>

enum BallColor { Blue=0, Brown, Cyan, Green, Red, Violet, Yellow, NumColors };

struct FieldPos
{
    int x;
    int y;
    FieldPos( int _x=-1, int _y=-1) : x(_x), y(_y) { }
    bool isValid() const { return (x != -1 && y != -1); }
    bool operator==(FieldPos other) const
    {
        return (x == other.x && y == other.y);
    }
};
Q_DECLARE_TYPEINFO(FieldPos, Q_MOVABLE_TYPE);
#endif
