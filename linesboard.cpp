/***************************************************************************
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

#include <qpainter.h>
#include <qcolor.h>
#include <qcursor.h>
#include <qkeycode.h>
#include <qtooltip.h>
#include <stdlib.h>

#include <kapplication.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

#include "linesboard.h"
#include "linesboard.moc"

/*
   Constructs a LinesBoard widget.
*/

LinesBoard::LinesBoard( BallPainter * abPainter, QWidget* parent, const char* name )
    : Field( parent, name )
{
  demoLabel = 0;
  bGameOver = false;
  anim = ANIM_NO;
  focusX = -1;
  focusY = -1;
//  waypos = 0;
//  waylen = 0;
//  jumpingRow = -1;
//  jumpingCol = -1;
  painting = 0;
  way = new Waypoints[NUMCELLSW*NUMCELLSH];

  bPainter = abPainter;

  setFocusPolicy( NoFocus );
  setBackgroundColor( gray );

  setMouseTracking( FALSE );
  setFixedSize(wHint(), hHint());

  timer = new QTimer(this);
  connect( timer, SIGNAL(timeout()), SLOT(timerSlot()) );
  timer->start( TIMERCLOCK, FALSE );

}

/*
   Destructor: deallocates memory for contents
*/

LinesBoard::~LinesBoard()
{
  timer->stop();
  delete timer;
  delete [] way;
}

void LinesBoard::startDemoMode()
{
    level = DEMO_LEVEL;
    rnd_demo = KRandomSequence(DEMO_SEQUENCE);
    bAllowMove = false;
}

void LinesBoard::adjustDemoMode(bool allowMove, bool off)
{
    bAllowMove = allowMove;
    if (off)
       level = -2;
}

void LinesBoard::demoAdjust(int a)
{
    rnd_demo.modulate(a);
}


void LinesBoard::placeBalls(int pnextBalls[BALLSDROP])
{
    for(int i=0; i < BALLSDROP; i++)
      nextBalls[i] = pnextBalls[i];

    nextBallToPlace = 0;
    placeBall();
}

void LinesBoard::placeBall(  )
{
    char* xx = (char*)malloc( sizeof(char)*NUMCELLSW*NUMCELLSH );
    char* yy = (char*)malloc( sizeof(char)*NUMCELLSW*NUMCELLSH );
    int empty = 0;
    for(int y=0; y<NUMCELLSH; y++)
      for(int x=0; x<NUMCELLSW; x++)
        if( getBall(x,y) == NOBALL )
        {
          xx[empty] = x;
          yy[empty] = y;
          empty++;
        };

    if (empty)
    {
      int color = nextBalls[nextBallToPlace];
      int pos = 0;
      if ((level == DEMO_LEVEL) || (level == 0))
      {
         pos  = random(empty);
      }
      else
      {
         int best_pos = 0;
         int best_score = level > 0 ? 1000: -1000;
         int maxtry = level > 0 ? level+1 : 1-level;
         for(int i=0;i<maxtry;i++)
         {
            int pos  = random(empty);
            int score = calcPosScore(xx[pos], yy[pos], color) - calcPosScore(xx[pos], yy[pos], NOBALL);
            if (((level > 0) && (score < best_score)) ||
                ((level < 0) && (score > best_score)))
            {
               best_pos = pos;
               best_score = score;
            }
         }
         pos = best_pos;
      }

      putBall( xx[pos], yy[pos], color );
      clearAnim();
      setAnim( xx[pos], yy[pos], ANIM_BORN );
			nextBallToPlace++;
      AnimStart(ANIM_BORN);
      free(xx);
      free(yy);
    }
    else
    {
      free(xx);
      free(yy);
      emit endGame();
    }
}


/*id LinesBoard::doAfterBalls() {
  erase5Balls();
  repaint(FALSE);
}
*/
/*
   Sets the size of the table
*/

int LinesBoard::width() { return CELLSIZE * NUMCELLSW; }
int LinesBoard::height() { return CELLSIZE * NUMCELLSH; }
int LinesBoard::wHint() { return width(); }
int LinesBoard::hHint() { return height(); }

void LinesBoard::setGameOver(bool b)
{
  bGameOver = b;
  focusX = -1;
  focusY = -1;
}


void LinesBoard::paintEvent( QPaintEvent* )
{
    QPainter *paint;
    KPixmap *pixmap = 0;
    if (bGameOver)
    {
       pixmap = new KPixmap();
       pixmap->resize(width(), height());
       paint = new QPainter( pixmap );
    }
    else
    {
       paint = new QPainter( this );
    }

    for( int y=0; y < NUMCELLSH; y++ ){
        for( int x=0; x < NUMCELLSW; x++ ){
          if( getBall(x,y) == NOBALL )
					{
            paint->drawPixmap(x*CELLSIZE, y*CELLSIZE, bPainter->GetBackgroundPix() );
					}
          else
          {
            paint->drawPixmap(x*CELLSIZE, y*CELLSIZE,
              bPainter->GetBall(getBall(x,y),animstep,getAnim(x,y)));
          }
        }
    }
    if (bGameOver)
    {
       paint->end();

       KPixmapEffect::fade(*pixmap, 0.5, Qt::black);

       QPainter p(this);
       p.drawPixmap(0,0, *pixmap);
       delete pixmap;

       QFont gameover_font = font();
       gameover_font.setPointSize(48);
       gameover_font.setBold(true);
       p.setFont(gameover_font);
       p.setPen(Qt::white);
       QString gameover_text = i18n("Game Over");
       p.drawText(0, 0, width(), height(), AlignCenter|Qt::WordBreak, gameover_text);
    }
    else
    {
      if ((focusX >= 0) && (focusX < NUMCELLSW) &&
          (focusY >= 0) && (focusY < NUMCELLSH))
      {
        QRect r;
        r.setX(focusX*CELLSIZE+2);
        r.setY(focusY*CELLSIZE+2);
        r.setWidth(CELLSIZE-4);
        r.setHeight(CELLSIZE-4);
        paint->setPen(QPen(Qt::DotLine));
        paint->drawRect(r);
      }
    }
    delete paint;
}

/*
   Handles mouse press events for the LinesBoard widget.
*/
void LinesBoard::mousePressEvent( QMouseEvent* e )
{
  if (bGameOver) return;
  if ((level == DEMO_LEVEL) && (!bAllowMove) && e->spontaneous()) return;

  int curRow = e->y() / CELLSIZE;
  int curCol = e->x() / CELLSIZE;

  placePlayerBall(curCol, curRow);
}

void LinesBoard::placePlayerBall(int curCol, int curRow)
{
  //check range
  if (!checkBounds( curCol, curRow ) )
    return;
//    if( running || anim != ANIM_NO )  return;
  if(anim != ANIM_JUMP && anim != ANIM_NO) return;
  if ( anim == ANIM_JUMP )
  {
    if ( getBall(curCol,curRow) == NOBALL )
    {
      if(existPath(jumpingCol, jumpingRow, curCol, curRow))
      {
        saveRandomState();
        rnd.modulate(jumpingCol);
        rnd.modulate(jumpingRow);
        rnd.modulate(curCol);
        rnd.modulate(curRow);
        saveUndo();
        AnimStart(ANIM_RUN);
      }
    }
    else
      AnimJump(curCol,curRow);
  }
  else
    AnimJump(curCol,curRow);
}

/*
   Move focus thingy
*/
void LinesBoard::moveFocus(int dx, int dy)
{
  if (bGameOver) return;
  if ((level == DEMO_LEVEL) && (!bAllowMove)) return;
  if (focusX == -1)
  {
    focusX = 0;
    focusY = 0;
  }
  else
  {
    focusX = (focusX + dx + NUMCELLSW) % NUMCELLSW;
    focusY = (focusY + dy + NUMCELLSH) % NUMCELLSH;
  }
  repaint(FALSE);
} 

void LinesBoard::moveLeft()
{
  moveFocus(-1, 0);
} 

void LinesBoard::moveRight()
{
  moveFocus(1, 0);
}

void LinesBoard::moveUp()
{
  moveFocus(0, -1);
}

void LinesBoard::moveDown()
{
  moveFocus(0, 1);
}

void LinesBoard::placePlayerBall()
{
  if (bGameOver) return;
  if ((level == DEMO_LEVEL) && (!bAllowMove)) return;
  placePlayerBall(focusX, focusY);
}

void LinesBoard::AnimJump(int x, int y ) {
  if ( getBall(x,y) != NOBALL )
    if (!( anim == ANIM_JUMP &&  jumpingCol == x && jumpingRow == y ))
      if ( AnimEnd() )
      {
        clearAnim();
        setAnim(x,y,ANIM_JUMP);
        jumpingCol = x;
        jumpingRow = y;
        AnimStart(ANIM_JUMP);
      }
}
void LinesBoard::AnimStart(int panim) {
  if (anim != ANIM_NO)
    AnimEnd();
  animstep = 0;
  animdelaystart = 1;
  switch(panim) {
    case ANIM_NO:
        break;
    case ANIM_BORN:
        animdelaystart=1;
        animmax = BOOMBALLS;
        break;
    case ANIM_JUMP:
        direction = -1;
        animstep = 4;
        animmax = PIXTIME -1;
        break;
    case ANIM_RUN:
        animdelaystart=3;
        // do first step on next timer;
        animdelaycount = 0;
        // animmax already set
        break;
    case ANIM_BURN:
        animdelaystart=1;
        animmax = FIREBALLS + FIREPIX - 1;
        break;
    default:
      ;
  }
  anim = panim;
  animdelaycount = animdelaystart;
}

int LinesBoard::AnimEnd( )
{
  if (anim == ANIM_NO ) return true;
  int oldanim = anim;
  anim = ANIM_NO;
  if (oldanim == ANIM_RUN) {
    if (animstep != animmax) {
      moveBall(way[animstep].x,way[animstep].y,way[animmax].x,way[animmax].y);
    }
    if ( erase5Balls() == 0 ) {
      emit endTurn();
      return true;
    }
    else
      return false;
  }
  else if ( oldanim == ANIM_BURN )
  {
    emit eraseLine( deleteAnimatedBalls() );
    repaint(FALSE);
    if ( nextBallToPlace < BALLSDROP )
    {
      placeBall();
      // continue with born
      return false;
    }
    else
    {
      emit userTurn();
      return true;
    }
  }
  else if ( oldanim == ANIM_BORN )
  {
    if ( erase5Balls() == 0 )
    {
      if ( freeSpace() > 0)
      {
        if ( nextBallToPlace < BALLSDROP )
        {
          placeBall();
          return false;
        }
        else
        {
          emit userTurn();
          return true;
        }
      }
      else
      {
        emit endGame();
        return false;
      }
    }
    else
    {
      // wait for user input
      emit userTurn();
      return true;
    }
  }
  emit userTurn();
  return true;
}

void LinesBoard::AnimNext() {
  if ( anim != ANIM_NO )
  {
    if ( anim == ANIM_JUMP ) {
      if ( (direction > 0 && animstep == animmax) || ( direction < 0 && animstep == 0))
        direction = -direction;
      animstep += direction;
      repaint(FALSE);
    } else {
      if ( animstep >= animmax )
        AnimEnd();
      else {
        animdelaycount--;
        if (animdelaycount <= 0) {
          if ( anim == ANIM_RUN )
            moveBall(way[animstep].x,way[animstep].y,way[animstep+1].x,way[animstep+1].y);
          animstep++;
          animdelaycount = animdelaystart;
          repaint( FALSE );
        }
      }
    }
  }
}
int LinesBoard::getAnim( int x, int y )
{
  return (( Field::getAnim(x,y) != ANIM_NO )? anim : ANIM_NO);
}

void LinesBoard::timerSlot()
{
  AnimNext();
}

int LinesBoard::erase5Balls()
{
    int nb=5;  // minimum balls for erasure

    bool bit_erase[NUMCELLSH][NUMCELLSW]; //bool array for balls, that must be erased
    for(int y=0; y<NUMCELLSH; y++)
      for(int x=0; x<NUMCELLSW; x++)
        bit_erase[y][x] = false;

    int color,newcolor;
    int count;
    //for horisontal

    for(int y=0; y<NUMCELLSH; y++) {
      count = 1;
      color = NOBALL;
      for(int x=0; x<NUMCELLSW; x++){
        if ( (newcolor = getBall(x,y)) == color) {
            if ( color != NOBALL) {
                count++;
                if ( count >= nb )
                    if ( count == nb )
                        for (int i = 0; i < nb; i++)
                            bit_erase[y][x-i] = true;
                    else bit_erase[y][x] = true;
            }
        } else {
            color = newcolor;
            count = 1;
        }
      }
    }

    //for vertical
    for(int x=0; x<NUMCELLSW; x++) {
      count = 0;
      color = NOBALL;
      for(int y=0; y<NUMCELLSH; y++){
        if ( (newcolor = getBall(x,y)) == color) {
            if ( color != NOBALL) {
                count++;
                if ( count >= nb )
                    if ( count == nb )
                        for (int i = 0; i < nb; i++)
                            bit_erase[y-i][x] = true;
                    else bit_erase[y][x] = true;
            }
        } else {
            color = newcolor;
            count = 1;
        }
      }
    }


    //for left-down to rigth-up diagonal
    for ( int xs = NUMCELLSW-1,ys = NUMCELLSH-nb; xs >= nb-1; ) {
        count = 0;
        color = NOBALL;
        for ( int x = xs, y = ys; x >= 0 && y < NUMCELLSH; x--, y++ ) {
            if ( (newcolor = getBall(x,y)) == color) {
                if ( color != NOBALL) {
                    count++;
                    if ( count >= nb )
                        if ( count == nb )
                            for (int i = 0; i < nb; i++)
                                bit_erase[y-i][x+i] = true;
                        else bit_erase[y][x] = true;
                }
            } else {
                color = newcolor;
                count = 1;
            }
        }
        if ( ys > 0 ) ys--; else xs--;
    }


    //for left-up to rigth-down diagonal
    for ( int xs = 0,ys = NUMCELLSH-nb; xs <= NUMCELLSW-nb; )
    {
        count = 0;
        color = NOBALL;
        for ( int x = xs, y = ys; x < NUMCELLSW && y < NUMCELLSH; x++, y++ )
        {
            if ( (newcolor = getBall(x,y)) == color)
            {
                if ( color != NOBALL)
                {
                    count++;
                    if ( count >= nb )
                        if ( count == nb )
                            for (int i = 0; i < nb; i++)
                                bit_erase[y-i][x-i] = true;
                        else
                          bit_erase[y][x] = true;
                }
            }
            else
            {
                color = newcolor;
                count = 1;
            }
        }
        if ( ys > 0 ) ys--; else xs++;
    }

    //remove all lines balls, that more than nb
    int cb=0;
    for(int y=0; y < NUMCELLSH; y++)
      for(int x=0; x < NUMCELLSW; x++)
      {
				if (bit_erase[y][x])
				{
        	setAnim(x,y,ANIM_YES);
        	cb++;
				}
				else
				{
          	setAnim(x,y,ANIM_NO);
        }
      }
    if ( cb > 0 )
    {
      AnimStart(ANIM_BURN);
      //emit eraseLine(cb);
    }

    return cb;
}


#define GO_EMPTY    4
#define GO_A        5
#define GO_B        6
#define GO_BALL     7

bool LinesBoard::existPath(int bx, int by, int ax, int ay)
{
  int dx[4]={1,-1, 0, 0};
  int dy[4]={0, 0, 1,-1};

  if (getBall(ax,ay) != NOBALL)
    return false;

  char pf[NUMCELLSH][NUMCELLSW];
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
      pf[y][x] = (getBall(x,y) == NOBALL) ? GO_EMPTY:GO_BALL;

  Waypoints lastchanged[2][NUMCELLSH*NUMCELLSW];

  int lastChangedCount[2];
  int currentchanged = 0;
  int nextchanged = 1;
  lastchanged[currentchanged][0].x = ax;
  lastchanged[currentchanged][0].y = ay;
  lastChangedCount[currentchanged] = 1;
  pf[ay][ax]=GO_A;
  pf[by][bx]=GO_B;

  // int expanded;
  bool B_reached = false;

  do
  {
    lastChangedCount[nextchanged] = 0;
    for(int dir=0; dir<4; dir++)
    {
      for ( int i = 0 ; i < lastChangedCount[currentchanged]; i++ )
      {
        int nx = lastchanged[currentchanged][i].x + dx[dir];
        int ny = lastchanged[currentchanged][i].y + dy[dir];
        if( (nx>=0) && (nx<NUMCELLSW) && (ny>=0) && (ny<NUMCELLSH) )
        {
          if( pf[ny][nx]==GO_EMPTY ||( nx==bx && ny==by ))
          {
            pf[ny][nx] = dir;
            lastchanged[nextchanged][lastChangedCount[nextchanged]].x = nx;
            lastchanged[nextchanged][lastChangedCount[nextchanged]].y = ny;
            lastChangedCount[nextchanged]++;
          };
          if( (nx==bx) && (ny==by) )
          {
            B_reached = true;
            break;
          }
        }
      }
    }
    nextchanged = nextchanged ^ 1;
    currentchanged = nextchanged ^ 1;
    //      currentchanged,lastChangedCount[currentchanged]);
  } while(!B_reached && lastChangedCount[currentchanged] != 0);

  if (B_reached) {
    AnimStart( ANIM_BLOCK);
    animmax = 0;
//    waypos = 0;
    int x, y,dir;
    for( x = bx, y = by; (dir = pf[y][x]) != GO_A;x -=dx[dir],y -= dy[dir]) {
     	way[animmax].x = x;
     	way[animmax].y = y;
    	animmax++;
    }
    way[animmax].x = x;
    way[animmax].y = y;
  }
  return B_reached;
}

void LinesBoard::undo()
{
  AnimEnd();
  restoreUndo();
  restoreRandomState();
  repaint( FALSE );
}

void LinesBoard::showDemoText(const QString &text)
{
  if (!demoLabel)
  {
     demoLabel = new QLabel(0, "demoTip", WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder | WStyle_Tool | WX11BypassWM );
     demoLabel->setMargin(1);
     demoLabel->setIndent(0);
     demoLabel->setAutoMask( FALSE );
     demoLabel->setFrameStyle( QFrame::Plain | QFrame::Box );
     demoLabel->setLineWidth( 1 );
     demoLabel->setAlignment( AlignHCenter | AlignTop );
     demoLabel->setPalette(QToolTip::palette());
     demoLabel->polish();
  }
  demoLabel->setText(text);
  demoLabel->adjustSize();
  QSize s = demoLabel->sizeHint();
  QPoint p = QPoint(x() + (width()-s.width())/2, y() + (height()-s.height())/2);
  demoLabel->move(mapToGlobal(p));
  demoLabel->show();
}

void LinesBoard::hideDemoText()
{
  if (demoLabel)
     demoLabel->hide();
}

void LinesBoard::demoClick(int x, int y)
{
  QPoint lDest = QPoint(x*CELLSIZE+(CELLSIZE/2), y*CELLSIZE+(CELLSIZE/2));
  QPoint dest = mapToGlobal(lDest);
  QPoint cur = QCursor::pos();
  for(int i = 0; i < 25;)
  {
     i++;
     QPoint p = cur + i*(dest-cur) / 25;
     QCursor::setPos(p);
     QApplication::flushX();
     QTimer::singleShot(80, this, SLOT(demoClickStep()));
     kapp->enter_loop();
  }
  QCursor::setPos(dest);
  QMouseEvent ev(QEvent::MouseButtonPress, lDest, dest, LeftButton, LeftButton);
  mousePressEvent(&ev);
}

void LinesBoard::demoClickStep()
{
  kapp->exit_loop();
}
