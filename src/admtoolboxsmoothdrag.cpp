#include "admtoolboxsmoothdrag.h"

#include <QDebug>
#include <QLayout>
#include <QScrollBar>
#include <QAbstractButton>

AdmScrollAreaSmoothDrag::AdmScrollAreaSmoothDrag(QWidget *parent) :
  QScrollArea(parent)
{
  // Setup the object
  acceptDrops();
  setMouseTracking(true);
  setMinimumHeight(ScrollMinHeight);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Initialize variables
  _scrollDir = ScrollOff;
  _timer = NULL;
  _pointDragMoveEvent = QPoint();
  _tbox = NULL;
}
void AdmScrollAreaSmoothDrag::setWidget(QWidget *widget)
{
  widget->installEventFilter(this);
  QScrollArea::setWidget(widget);
}

bool AdmScrollAreaSmoothDrag::eventFilter(QObject *obj, QEvent *event)
{
  if(event->type() == QEvent::Resize && obj == widget())
  {
    bool parentFound = false;
    if(NULL == _tbox)
    {
      QObject *p = NULL;
      AdmToolBoxSmoothDrag *o = NULL;
      p = this;
      while(parentFound == false)
      {
        p = p->parent();
        if(NULL == p)
          return false;
        o = qobject_cast<AdmToolBoxSmoothDrag *>(p);
        if(NULL != o)
        {
          _tbox = o;
          parentFound = true;
        }
      }
    } else {
      parentFound = true;
    }
    if(parentFound)
    {
      int left=0;
      int top=0;
      int right=0;
      int bottom=0;
      int addWidth=0;
      _tbox->getContentsMargins(&left,&top,&right,&bottom);
      addWidth = left+right;
      getContentsMargins(&left,&top,&right,&bottom);
      addWidth += left+right;
      _tbox->setMinimumWidth(widget()->width()+addWidth);
    }
  }
  return false;
}

void AdmScrollAreaSmoothDrag::dragEnterEvent(QDragEnterEvent *event)
{
  // Make sure the timer is not going and we are scrollable
  if(NULL == _timer && _scrollDir == ScrollOff && verticalScrollBar()->isVisible() && height() >= ScrollMinHeight)
  {
    // Get the mouse position
    _pointDragMoveEvent = event->pos();

    // Test for upper region and start the scroll timer scrolling up
    QRect r(0,0,width(),ScrollRegionHeight);
    if(r.contains(_pointDragMoveEvent))
    {
      _scrollDir = ScrollUp;
      _timer = new QTimer();
      connect(_timer,SIGNAL(timeout()),this,SLOT(sTickTock()));
      _timer->start(ScrollTimerTimout);
    } else {
      // Test for lower region and start the scroll timer scrolling down
      r = QRect(0,height()-ScrollRegionHeight,width(),ScrollRegionHeight);
      if(r.contains(_pointDragMoveEvent))
      {
        _scrollDir = ScrollDown;
        _timer = new QTimer();
        connect(_timer,SIGNAL(timeout()),this,SLOT(sTickTock()));
        _timer->start(ScrollTimerTimout);
      }
    }
  }
}

void AdmScrollAreaSmoothDrag::sDragEnterEvent(AdmExtensionWidget *obj, QDragEnterEvent *event)
{
  // Make sure the timer is not going and we are scrollable
  if(_timer == NULL && _scrollDir == ScrollOff && verticalScrollBar()->isVisible() && height() >= ScrollMinHeight)
  {
    // Get the mouse position
    _pointDragMoveEvent = mapTo(this,event->pos());
    QPoint eventPos = event->pos();

    // Determine the region of the mouse position
    if((eventPos.ry()+obj->y())-verticalScrollBar()->value() <= ScrollRegionHeight)
    {
      // Test for upper region and start the scroll timer scrolling up
      _scrollDir = ScrollUp;
      _timer = new QTimer();
      connect(_timer,SIGNAL(timeout()),this,SLOT(sTickTock()));
      _timer->start(ScrollTimerTimout);
    } else if (rect().height() + verticalScrollBar()->value() - (eventPos.ry()+obj->y()) <= ScrollRegionHeight) {
      // Test for lower region and start the scroll timer scrolling down
      _scrollDir = ScrollDown;
      _timer = new QTimer();
      connect(_timer,SIGNAL(timeout()),this,SLOT(sTickTock()));
      _timer->start(ScrollTimerTimout);
    }
  }
}

void AdmScrollAreaSmoothDrag::sTickTock()
{
  // Get the current mouse position and map it to this widget
  QPoint p = mapFromGlobal(QCursor::pos());

  switch(_scrollDir)
  {
    case ScrollOff:
    {
      // Disable scrolling and stop the Timer
      disconnect(_timer,SIGNAL(timeout()),this,SLOT(sTickTock()));
      _timer->stop();
      _timer->deleteLater();
      _timer = NULL;
      break;
    }
    case ScrollUp:
    {
      // Determine if the mouse is still in the upper region
      QRect r(0,0,width(),ScrollRegionHeight);
      if(r.contains(p))
      {
        // Mouse is still in the region, scroll up
        verticalScrollBar()->setValue(verticalScrollBar()->value()-ScrollByValue);

        // If the vertical scrollbar has reached the top, stop the scrolling timer
        if(verticalScrollBar()->value() <= verticalScrollBar()->minimum())
          _scrollDir = ScrollOff;
      } else {
        // Mouse has left the region, stop the scrolling timer
        _scrollDir = ScrollOff;
      }
      break;
    }
    case ScrollDown:
    {
      // Determine if the mouse is still in the lower region
      QRect r(0,height()-ScrollRegionHeight,width(),ScrollRegionHeight);
      if(r.contains(p))
      {
        // Mouse is still in the region, scroll down
        verticalScrollBar()->setValue(verticalScrollBar()->value()+ScrollByValue);

        // If the vertical scrollbar has reached the bottom, stop the scrolling timer
        if(verticalScrollBar()->value() >= verticalScrollBar()->maximum())
          _scrollDir = ScrollOff;
      } else {
        // Mouse has left the region, stop the scrolling timer
        _scrollDir = ScrollOff;
      }
      break;
    }
    default:
      break;
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

AdmToolBoxSmoothDrag::AdmToolBoxSmoothDrag(QWidget *parent) :
  QToolBox(parent)
{
  // Setup the object
  acceptDrops();
  setMouseTracking(true);

  // Initialize variables
  _timer = NULL;
  _pointDragMoveEvent = QPoint();
}

void AdmToolBoxSmoothDrag::dragEnterEvent(QDragEnterEvent *event)
{
  // Accept the event so we can get the dragMoveEvents
  event->acceptProposedAction();
}

void AdmToolBoxSmoothDrag::dragMoveEvent(QDragMoveEvent *event)
{
  // If the timer has been started, cancel it.
  if(NULL !=_timer)
  {
    _timer->blockSignals(true);
    _timer->stop();
    _timer = NULL;
    _pointDragMoveEvent = QPoint();
  }
  // Start the one-shot timer so we can get a delayed response to hovering.
  _pointDragMoveEvent = event->pos();
  _timer = new QTimer();
  _timer->setSingleShot(true);
  connect(_timer,SIGNAL(timeout()),this,SLOT(sDelayedDragMoveEvent()));
  _timer->start(DragTimerTimeout);
}

void AdmToolBoxSmoothDrag::sDelayedDragMoveEvent()
{
  // Make sure the timer is valid and we have a mouse position.
  if(NULL != _timer && _pointDragMoveEvent.isNull() == false)
  {
    // Stop the timer
    disconnect(_timer,SIGNAL(timeout()),this,SLOT(sDelayedDragMoveEvent()));
    _timer->stop();

    // Loop through the buttons of this widget and determine which button the
    // mouse is hovering over. The x value should be the widget location in this
    // QToolBox object
    int x = -1;
    foreach(QAbstractButton *btn, findChildren<QAbstractButton *>())
    {
      ++x;

      // Determine the button's region in relation to this QToolBox object
      QPoint mp = mapTo(this, btn->pos());
      QRect r(  mp.rx(),
                mp.ry(),
                btn->rect().width(),
                btn->rect().height()
      );

      // If the mouse position is hovering over this button, then select this
      // widget (x) and break out of the loop
      if(r.contains(_pointDragMoveEvent,false))
      {
        setCurrentWidget(widget(x));
        break;
      }
    }

    // Uninitialize event values
    _timer = NULL;
    _pointDragMoveEvent = QPoint();
  }
}


void AdmToolBoxSmoothDrag::dragLeaveEvent(QDragLeaveEvent *event)
{
  Q_UNUSED(event)

  // Kill the timer if it's still active
  if(NULL !=_timer)
  {
    _timer->blockSignals(true);
    _timer->stop();
    _timer = NULL;
    _pointDragMoveEvent = QPoint();
  }
}
