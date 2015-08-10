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
  if(event->button() == Qt::LeftButton)
  {
    bool found = false;
    AdmCallWidget *cw = NULL;
    QObject *obj = this;
    // Find the AdmCallWidget. We should reach this through our parent lineage
    while(!found)
    {
      obj = obj->parent();
      if(obj)
      {
        cw = qobject_cast<AdmCallWidget *>(obj);
        if(cw)
        {
          found = true;
          break;
        }
      } else {
        break;
      }
    }
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
}
