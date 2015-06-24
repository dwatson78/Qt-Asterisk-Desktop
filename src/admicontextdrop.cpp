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
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);

    QPalette p = ui->_text->palette();
    QColor c = p.color(ui->_text->backgroundRole());
    c = c.darker(150);
    p.setColor(ui->_text->backgroundRole(), c);
    ui->_text->setAutoFillBackground(true);
    ui->_text->setPalette(p);
    ui->_icon->setAutoFillBackground(true);
    ui->_icon->setPalette(p);
  }
}

void AdmIconTextDrop::dragLeaveEvent(QDragLeaveEvent *event)
{
  this->setFrameShape(QFrame::NoFrame);
  QPalette p = ui->_text->palette();
  QColor c = p.color(ui->_text->backgroundRole());
  c = c.lighter(150);
  p.setColor(ui->_text->backgroundRole(), c);
  ui->_text->setAutoFillBackground(true);
  ui->_text->setPalette(p);
  ui->_icon->setAutoFillBackground(true);
  ui->_icon->setPalette(p);
}

void AdmIconTextDrop::dropEvent(QDropEvent *event)
{
  this->setFrameShape(QFrame::NoFrame);
  QPalette p = ui->_text->palette();
  QColor c = p.color(ui->_text->backgroundRole());
  c = c.lighter(150);
  p.setColor(ui->_text->backgroundRole(), c);
  ui->_text->setAutoFillBackground(true);
  ui->_text->setPalette(p);
  ui->_icon->setAutoFillBackground(true);
  ui->_icon->setPalette(p);

  const MimeDataAstCall *mime = qobject_cast<const MimeDataAstCall *>(event->mimeData());
  if(NULL != mime && mime->hasAdmCallWidget())
  {
    //AdmIconTextDrop *o = this;
    AdmCallWidget *cw = mime->admCallWidget();
    cw->sStartCallXfer(this);
  }
}
