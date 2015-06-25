#include "admicontextdrop.h"
#include "ui_admicontext.h"

#include <QPalette>
#include "draggabletoolbutton.h"

AdmIconTextDrop::AdmIconTextDrop(QWidget *parent) :
  AdmIconText(parent)
{
  _initSelf();
}
AdmIconTextDrop::AdmIconTextDrop(const QPixmap &icon, const QString &text, QWidget *parent) :
  AdmIconText(icon, text, parent)
{
  _initSelf();
}
void AdmIconTextDrop::_initSelf()
{
  setAcceptDrops(true);
}

void AdmIconTextDrop::dragEnterEvent(QDragEnterEvent *event)
{
  emit sigDragEnterEvent(this,event);
  //meDataAstCall
  const MimeDataAstCall *mime = qobject_cast<const MimeDataAstCall *>(event->mimeData());
  if(NULL != mime && mime->hasAdmCallWidget())
  {
    // Don't allow a call to transfer to channel in the same call
    AdmCallWidget *cw = mime->admCallWidget();
    bool found = false;
    cw->getChannelForDevice("SIP",this->getText(),&found);
    if(found)
      return;

    // Accept the drop and change appearance
    event->acceptProposedAction();

    ui->_text->setAutoFillBackground(true);
    ui->_text->setBackgroundRole(QPalette::Highlight);
    ui->_text->setForegroundRole(QPalette::HighlightedText);
  }
}
void AdmIconTextDrop::dragMoveEvent(QDragMoveEvent *event)
{
  emit sigDragMoveEvent(this,event);
}

void AdmIconTextDrop::dragLeaveEvent(QDragLeaveEvent *event)
{
  emit sigDragLeaveEvent(this, event);
  ui->_text->setAutoFillBackground(true);
  ui->_text->setBackgroundRole(QPalette::Button);
  ui->_text->setForegroundRole(QPalette::ButtonText);

  event->accept();
}

void AdmIconTextDrop::dropEvent(QDropEvent *event)
{
  ui->_text->setAutoFillBackground(true);
  ui->_text->setBackgroundRole(QPalette::Button);
  ui->_text->setForegroundRole(QPalette::ButtonText);

  const MimeDataAstCall *mime = qobject_cast<const MimeDataAstCall *>(event->mimeData());
  if(NULL != mime && mime->hasAdmCallWidget())
  {
    //AdmIconTextDrop *o = this;
    AdmCallWidget *cw = mime->admCallWidget();
    cw->sStartCallXfer(this);
    event->acceptProposedAction();
  }
}
