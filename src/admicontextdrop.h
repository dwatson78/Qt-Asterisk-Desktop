#ifndef ADMICONTEXTDROP_H
#define ADMICONTEXTDROP_H

#include "admicontext.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>

class AdmIconTextDrop : public AdmIconText
{
  Q_OBJECT

public:
  explicit AdmIconTextDrop(QWidget *parent = 0);
  explicit AdmIconTextDrop(const QPixmap &icon, const QString &text, QWidget *parent = 0);

public slots:

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dropEvent(QDropEvent *event);

signals:

private:
  void _initSelf();
  
};

#endif // ADMICONTEXTDROP_H
