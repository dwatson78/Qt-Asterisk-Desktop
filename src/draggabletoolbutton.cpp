#include "draggabletoolbutton.h"

#include <QDebug>
#include <QDrag>
#include <QIcon>

#include "admcallwidget.h"

MimeDataAstCall::MimeDataAstCall() :
  QMimeData()
{
  _admCallWidget = 0;
}

AdmCallWidget* MimeDataAstCall::admCallWidget() const
{
  return _admCallWidget;
}

void MimeDataAstCall::setAdmCallWidget(AdmCallWidget *admCallWidget)
{
  _admCallWidget = admCallWidget;
}

bool MimeDataAstCall::hasAdmCallWidget() const
{
  return (NULL != _admCallWidget);
}

DraggableToolButton::DraggableToolButton(QWidget *parent) :
  QToolButton(parent)
{
  QIcon icon;
  icon.addPixmap(QPixmap(":/icons/call-transfer.png"));
  setIcon(icon);
}

void DraggableToolButton::mousePressEvent(QMouseEvent *event)
{
  AdmCallWidget *cw = qobject_cast<AdmCallWidget *>(parent());
  if(NULL != cw)
  {
    QDrag *drag = new QDrag(this);
    drag->setPixmap(this->icon().pixmap(24,24));
    MimeDataAstCall *mimeData = new MimeDataAstCall;
    mimeData->setAdmCallWidget(cw);
    drag->setMimeData(mimeData);
    drag->exec();
  }
}

/*void DraggableToolButton::dragMoveEvent(QDragMoveEvent *event)
{
  event->setDropAction(Qt::MoveAction);
  event->accept();
}*/
