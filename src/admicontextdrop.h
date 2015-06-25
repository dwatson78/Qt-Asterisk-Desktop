#ifndef ADMICONTEXTDROP_H
#define ADMICONTEXTDROP_H

#include "admicontext.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

class AdmIconTextDrop : public AdmIconText
{
  Q_OBJECT

public:
  explicit AdmIconTextDrop(QWidget *parent = 0);
  explicit AdmIconTextDrop(const QPixmap &icon, const QString &text, QWidget *parent = 0);

public slots:

protected:
  void dragEnterEvent (QDragEnterEvent  *event);
  void dragMoveEvent  (QDragMoveEvent   *event);
  void dragLeaveEvent (QDragLeaveEvent  *event);
  void dropEvent      (QDropEvent       *event);

signals:
  void sigDragEnterEvent(AdmIconTextDrop *obj, QDragEnterEvent  *event);
  void sigDragMoveEvent (AdmIconTextDrop *obj, QDragMoveEvent   *event);
  void sigDragLeaveEvent(AdmIconTextDrop *obj, QDragLeaveEvent  *event);

private:
  void _initSelf();
  
};

#endif // ADMICONTEXTDROP_H
