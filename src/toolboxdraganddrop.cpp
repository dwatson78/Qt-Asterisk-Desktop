#include "toolboxdraganddrop.h"

#include <QDebug>
#include <QScrollBar>
#include <QAbstractButton>

ScrollAreaAutoScrollOnDrag::ScrollAreaAutoScrollOnDrag(QWidget *parent) :
  QScrollArea(parent)
{
  acceptDrops();
  setMouseTracking(true);
}

void ScrollAreaAutoScrollOnDrag::mouseMoveEvent(QMouseEvent *event)
{
  //qDebug() << event->type();
}
void ScrollAreaAutoScrollOnDrag::dragEnterEvent(QDragEnterEvent *event)
{
  //qDebug() << "ScrollAreaAutoScrollOnDrag::dragEnter";
  //event->accept();
}
void ScrollAreaAutoScrollOnDrag::dragLeaveEvent(QDragLeaveEvent *event)
{
  //qDebug() << "ScrollAreaAutoScrollOnDrag::dragLeave";
}

void ScrollAreaAutoScrollOnDrag::sDragEnterEvent(AdmIconTextDrop *obj, QDragEnterEvent *event)
{
  QPoint eventPos = event->pos();

  /*qDebug() << "ScrollAreaAutoScrollOnDrag::sDragEnterEvent";
  qDebug() << QString("eventPos.ry(): %1")
              .arg(eventPos.ry());
  qDebug() << QString("obj->y(): %1")
              .arg(obj->y());
  qDebug() << QString("verticalScrollBar()->value(): %1")
              .arg(verticalScrollBar()->value());
  qDebug() << QString("ry + y - scroll: %1")
              .arg(eventPos.ry()+obj->y()-verticalScrollBar()->value());
  qDebug() << QString("(obj->height()*1.5): %1")
              .arg((obj->height()*1.5));
  qDebug() << QString("rect().height(): %1")
              .arg(rect().height());
  qDebug() << QString("rect - (ry + y): %1")
              .arg(rect().height() - (eventPos.ry()+obj->y()));
  */
  if((eventPos.ry()+obj->y())-verticalScrollBar()->value() <= (obj->height()*1.5))
  {
    qDebug() << "Scroll Up";
    verticalScrollBar()->setValue(verticalScrollBar()->value()-(obj->height()*1.5));
    usleep(150);
  }
  if(rect().height() + verticalScrollBar()->value() - (eventPos.ry()+obj->y()) <= (obj->height()*1.5))
  {
    qDebug() << "Scroll Down";
    verticalScrollBar()->setValue(verticalScrollBar()->value()+(obj->height()*1.5));
    usleep(150);
  }
  //event->accept();
}

void ScrollAreaAutoScrollOnDrag::sDragMoveEvent(AdmIconTextDrop *obj, QDragMoveEvent *event)
{
  //qDebug() << "ScrollAreaAutoScrollOnDrag::sDragMoveEvent";
}

void ScrollAreaAutoScrollOnDrag::sDragLeaveEvent(AdmIconTextDrop *obj, QDragLeaveEvent *event)
{
  //qDebug() << "ScrollAreaAutoScrollOnDrag::sDragLeaveEvent";
}

void ScrollAreaAutoScrollOnDrag::enterEvent(QEvent *event)
{
  //qDebug() << "ScrollAreaAutoScrollOnDrag::enterEvent";
}
void ScrollAreaAutoScrollOnDrag::leaveEvent(QEvent *event)
{
  //qDebug() << "ScrollAreaAutoScrollOnDrag::leaveEvent";
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

ToolBoxDragAndDrop::ToolBoxDragAndDrop(QWidget *parent) :
  QToolBox(parent)
{
  acceptDrops();
  setMouseTracking(true);
  _timer = NULL;
  _event = NULL;
}

void ToolBoxDragAndDrop::dragEnterEvent(QDragEnterEvent *event)
{
  event->acceptProposedAction();
}

void ToolBoxDragAndDrop::dragMoveEvent(QDragMoveEvent *event)
{
  if(NULL !=_timer)
  {
    _timer->blockSignals(true);
    _timer->stop();
    _timer = NULL;
    _event = NULL;
  }
  _event = event;
  _timer = new QTimer();
  _timer->setSingleShot(true);
  connect(_timer,SIGNAL(timeout()),this,SLOT(sDelayedDragMoveEvent()));
  _timer->start(250);
}
void ToolBoxDragAndDrop::sDelayedDragMoveEvent()
{
  qDebug() << "ToolBoxDragAndDrop::dragMove";
  _timer->blockSignals(true);
  _timer->stop();
  _timer = NULL;
  QPoint p = _event->pos();

  qDebug() << QString("event->pos(): %1,%2")
              .arg(p.rx())
              .arg(p.ry());
  int x = -1;
  foreach(QAbstractButton *btn, findChildren<QAbstractButton *>())
  {
    ++x;
    qDebug() << QString("btn->pos(): %1,%2")
                .arg(QString::number(btn->pos().rx()))
                .arg(QString::number(btn->pos().ry()));

    QPoint mp = mapTo(this,btn->pos());
    QRect r(  /*btn->pos().x()-6,*/
              mp.rx()-6,
              /*btn->pos().y()-2,*/
              mp.ry()-6,
              btn->rect().width()+6,
              btn->rect().height()+6
    );
    qDebug() << QString("Rectangle: x1: %1, y1: %2, width: %3, height: %4")
                .arg(r.x())
                .arg(r.y())
                .arg(r.width())
                .arg(r.height())
                ;
    if(r.contains(p,false))
    {
      qDebug() << QString("Button found!!");
      setCurrentWidget(widget(x));
      break;
    }
  }
  _event = NULL;
}


void ToolBoxDragAndDrop::dragLeaveEvent(QDragLeaveEvent *event)
{
  if(NULL !=_timer)
  {
    _timer->blockSignals(true);
    _timer->stop();
    _timer = NULL;
    _event = NULL;
  }
}
