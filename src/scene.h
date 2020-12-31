/*
    This file is part of the KDE project "KLines"

    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KL_SCENE_H
#define KL_SCENE_H

#include <QGraphicsScene>
#include <QRandomGenerator>

#include "commondefs.h"

static const int FIELD_SIZE=9;

class KLinesAnimator;
class BallItem;
class PreviewItem;
class QGraphicsRectItem;
class KGamePopupItem;

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
     *  Brings in next three balls to scene
     */
    void nextThreeBalls();
    /**
     *  Show/Hide the preview zone
     */
    void setPreviewZoneVisible( bool visible );
    /**
     *  This score points will be added as an additional bonus to
     *  every score resulted from ball erasing event.
     *  For example 1 score point is added if the game is played with
     *  hidden preview widget.
     *  By default no bonus is added.
     */
    void setBonusScorePoints( int points ) { m_bonusScore = points; }
    /**
     *  Returns colors of the 3 balls in the next turn
     */
    QList<BallColor> nextColors() const { return m_nextColors; }
    /**
     *  Returns ballitem in field position pos or 0 if there
     *  is no item there
     */
    BallItem* ballAt( FieldPos pos ) { return m_field[pos.x][pos.y]; }
    /**
     * Overloaded above function
     */
    BallItem* ballAt( int x, int y ) { return m_field[x][y]; }
    /**
     *  Field coords to pixel coords
     */
    inline QPointF fieldToPix(FieldPos fpos) const
    {
        return QPointF( m_playFieldRect.x() + m_playFieldBorderSize + fpos.x*m_cellSize,
                        m_playFieldRect.y() + m_playFieldBorderSize + fpos.y*m_cellSize );
    }
    /**
     *  Pixel coords to field coords
     */
    inline FieldPos pixToField( QPointF p ) const
    {
        return FieldPos(static_cast<int>(( p.x()-m_playFieldRect.x()-m_playFieldBorderSize )/m_cellSize),
                        static_cast<int>(( p.y()-m_playFieldRect.y()-m_playFieldBorderSize )/m_cellSize));
    }
public Q_SLOTS:
    /**
     *  Starts new game
     */
    void startNewGame();
    /**
     * Ends game
     */
    void endGame();
    /**
     *  Ends current and starts next turn explicitly
     */
    void endTurn();
    /**
     *  Undoes one move
     */
    void undo();
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
Q_SIGNALS:
    void scoreChanged(int);
    void stateChanged(const QString &);
    void gameOver(int);
private Q_SLOTS:
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
     *  This function takes one of two actions:
     *  If there's a ball at fpos, it will be selected.
     *  Otherwise if the cell at fpos is empty and there's
     *  a selected ball in some other cell it will be moved to fpos
     *  (if the move is possible, of course)
     */
    void selectOrMove(FieldPos fpos );
    /**
     *  Saves game state information to be used during undo
     */
    void saveUndoInfo();
    /** Does some actions upon game over. Called from various places where
     * it is clear that game is now over. emits gameOver(int) signal
     */
    void gameOverHandler();

    void drawBackground( QPainter*, const QRectF& ) override;
    void mousePressEvent( QGraphicsSceneMouseEvent* ) override;

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
    QRandomGenerator m_randomSeq;
    /**
     * Area of playfield (with border included - if any exists in theme)
     */
    QRect m_playFieldRect;
    /**
     * Size of a playfield border.
     * Equals 0 if there's no border element in current theme
     */
    int m_playFieldBorderSize;

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
     *  Bonus points added to score upon ball erasing
     *  @see setBonusScorePoints()
     */
    int m_bonusScore;
    /**
     *  Cell size in pixels
     */
    int m_cellSize;
    /**
     *  Is true if preview zone is visible
     */
    bool m_previewZoneVisible;
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
    /**
     *  Item which displays next balls preview
     */
    PreviewItem *m_previewItem;
    /**
     * Item to show popup messages to user
     */
    KGamePopupItem *m_popupItem;
    /**
     *  Struct for holding game state - used on undos
     */
    struct UndoInfo
    {
        int numFreeCells;
        int score;
        QList<BallColor> nextColors;
        BallColor fcolors[FIELD_SIZE][FIELD_SIZE];
    };
    /**
     *  Holds game state for undo.
     *  It is saved before every new turn
     */
    UndoInfo m_undoInfo;
    bool m_gameOver;
};

#endif
