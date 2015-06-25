#ifndef TOOLBOXDRAGANDDROP_H
#define TOOLBOXDRAGANDDROP_H

#include <QScrollArea>
#include <QToolBox>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QTimer>

#include "admicontextdrop.h"

class ScrollAreaAutoScrollOnDrag : public QScrollArea
{
  Q_OBJECT
public:
  explicit ScrollAreaAutoScrollOnDrag(QWidget *parent = 0);

protected:
  void dragEnterEvent(QDragEnterEvent *event);

  void dragLeaveEvent(QDragLeaveEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);
signals:

public slots:
  void sDragEnterEvent(AdmIconTextDrop *obj, QDragEnterEvent *event);
  void sDragMoveEvent(AdmIconTextDrop *obj, QDragMoveEvent *event);
  void sDragLeaveEvent(AdmIconTextDrop *obj, QDragLeaveEvent *event);
};

class ToolBoxDragAndDrop : public QToolBox
{
  Q_OBJECT
public:
  explicit ToolBoxDragAndDrop(QWidget *parent = 0);
  
protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);

signals:
  
public slots:
  void sDelayedDragMoveEvent();
private:
  QTimer *_timer;
  QDragMoveEvent *_event;
  
};

#endif // TOOLBOXDRAGANDDROP_H
