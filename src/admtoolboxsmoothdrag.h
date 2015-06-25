#ifndef ADMTOOLBOXSMOOTHDRAG_H
#define ADMTOOLBOXSMOOTHDRAG_H

#include <QScrollArea>
#include <QToolBox>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QPoint>
#include <QTimer>
#include <QCursor>

#include "admicontextdrop.h"

#define SCROLL_UP = 1
#define SCROLL_DOWN = 1

class AdmScrollAreaSmoothDrag : public QScrollArea
{
  Q_OBJECT
public:
  explicit AdmScrollAreaSmoothDrag(QWidget *parent = 0);

  static const int ScrollOff  = 0;
  static const int ScrollUp   = 1;
  static const int ScrollDown = 2;
  static const int ScrollRegionHeight = 30;
  static const int ScrollByValue = 10;
  static const int ScrollTimerTimout = 50; //50 milliseconds

protected:
  void dragEnterEvent (QDragEnterEvent  *event);
signals:

public slots:
  void sDragEnterEvent(AdmIconTextDrop  *obj, QDragEnterEvent *event);
  void sTickTock();
private:
  int       _scrollDir;
  QTimer  * _timer;
  QPoint    _pointDragMoveEvent;

};

class AdmToolBoxSmoothDrag : public QToolBox
{
  Q_OBJECT
public:
  explicit AdmToolBoxSmoothDrag(QWidget *parent = 0);

  static const int DragTimerTimeout = 250; //250 milliseconds

protected:
  void dragEnterEvent (QDragEnterEvent  *event);
  void dragMoveEvent  (QDragMoveEvent   *event);
  void dragLeaveEvent (QDragLeaveEvent  *event);

signals:
  
public slots:
  void sDelayedDragMoveEvent();

private:
  QTimer          *_timer;
  QPoint          _pointDragMoveEvent;
};

#endif // ADMTOOLBOXSMOOTHDRAG_H
