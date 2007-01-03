/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KLines"
 *
 * KLines is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KLines is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KAtomic; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#ifndef KL_SCENE_H
#define KL_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <KRandomSequence>

#include "commondefs.h"

static const int FIELD_SIZE=9;

class KLinesAnimator;
class BallItem;
class QGraphicsRectItem;

/**
 *  Displays and drives the game
 */
class KLinesScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit KLinesScene( QObject *parent );
    ~KLinesScene();

    /**
     *  Resizes scene
     */
    void resizeScene( int width, int height );
    /**
     *  Returns colors of the 3 balls in the next turn
     */
    QList<BallColor> nextColors() const { return m_nextColors; }
    /**
     *  Brings in next three balls to scene
     */
    void nextThreeBalls();
    /**
     *  Returns ballitem in field position pos or 0 if there
     *  is no item there
     */
    BallItem* ballAt( const FieldPos& pos ) { return m_field[pos.x][pos.y]; }
    /**
     * Overloaded above function
     */
    BallItem* ballAt( int x, int y ) { return m_field[x][y]; }
    /**
     *  Field coords to pixel coords
     */
    inline QPointF fieldToPix(const FieldPos& fpos) const {
        return QPointF( fpos.x*32+2, fpos.y*32+2 );
    }
    /**
     *  Pixel coords to field coords
     */
    inline FieldPos pixToField( const QPointF& p ) const { 
        return FieldPos(static_cast<int>(p.x()/32), static_cast<int>(p.y()/32)); }
public slots:
    /**
     *  Starts new game
     */
    void startNewGame();
    /**
     *  Moves keyboard-playing focus rect to the left 
     */
    void moveFocusLeft();
    /**
     *  Moves keyboard-playing focus rect to the right 
     */
    void moveFocusRight();
    /**
     *  Moves keyboard-playing focus rect to the up 
     */
    void moveFocusUp();
    /**
     *  Moves keyboard-playing focus rect to the down 
     */
    void moveFocusDown();
    /**
     *  Takes corresponding action on cell under focus rect
     */
    void cellSelected();
signals:
    void scoreChanged(int);
    void nextColorsChanged();
    void gameOver(int);
private slots:
    void moveAnimFinished();
    void removeAnimFinished();
    void bornAnimFinished();
private:
    /**
     *  Creates a ball and places it in random free cell
     *  @param c color of the ball
     *  @return ball placed
     */
    BallItem* randomlyPlaceBall(BallColor c);
    /**
     *  Searches for 5 or more balls in a row and deletes them from field
     */
    void searchAndErase();
    /**
     *  This fuction takes one of two actions:
     *  If there's a ball at fpos, it will be selected.
     *  Otherwise if the cell at fpos is empty and there's 
     *  a selected ball in some other cell it will be moved to fpos
     *  (if the move is possible, of course)
     */
    void selectOrMove( const FieldPos& fpos );

    virtual void drawBackground( QPainter*, const QRectF& );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );

    /**
     *  This array represents the play field.
     *  Each cell holds the pointer to BallItem
     *  or 0 if there's no ball in that cell
     */
    BallItem* m_field[FIELD_SIZE][FIELD_SIZE];
    /**
     *  Used to start game animations
     *  This object knows how to do some ball animations
     */
    KLinesAnimator* m_animator;
    /**
     * We need random numbers in this game
     */
    KRandomSequence m_randomSeq;
    /**
     *  Position of selected ball (-1,-1) if none
     */
    FieldPos m_selPos;
    /**
     *  Number of free cells in the field
     */
    int m_numFreeCells;
    /**
     *  Current game score
     */
    int m_score;
    /**
     *  Varable which is needed for little trick (tm).
     *  Read more about it in removeAnimFinished() slot
     */
    bool m_placeBalls;
    /**
     *  Items pending for removal after remove-anim finishes
     */
    QList<BallItem*> m_itemsToDelete;
    /**
     *  Colors of the next turn's balls
     */
    QList<BallColor> m_nextColors;
    /**
     *  Keyboard-playing focus indication
     */
    QGraphicsRectItem *m_focusItem;
};

class KLinesView : public QGraphicsView
{
public:
    KLinesView( KLinesScene* scene, QWidget *parent );
private:
    void resizeEvent(QResizeEvent *);
};

#endif
