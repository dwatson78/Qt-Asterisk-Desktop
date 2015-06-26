#include "admextensionwidget.h"
#include "ui_admextensionwidget.h"

#include <QPalette>
#include "draggabletoolbutton.h"

AdmExtensionWidget::AdmExtensionWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmExtensionWidget)
{
  ui->setupUi(this);
  acceptDrops();
  setMouseTracking(true);
  ui->_icon->setVisible(false);
}

AdmExtensionWidget::~AdmExtensionWidget()
{
  emit destroying(this);
  delete ui;
}

const QString AdmExtensionWidget::getExten()
{
  return ui->_exten->text();
}
void AdmExtensionWidget::setExten(const QString &exten)
{
  ui->_exten->setText(exten);
}
void AdmExtensionWidget::setExten(const QVariant &exten)
{
  ui->_exten->setText(exten.toString());
}

const QString AdmExtensionWidget::getText()
{
  return ui->_desc->text();
}
void AdmExtensionWidget::setText(const QString &text)
{
  ui->_desc->setText(text);
}

const QPixmap AdmExtensionWidget::getPixmap()
{
  return QPixmap(this->ui->_icon->pixmap()->copy(this->ui->_icon->pixmap()->rect()));
}
void AdmExtensionWidget::setPixmap(const QPixmap &pixmap)
{
  this->ui->_icon->setPixmap(pixmap);
  this->ui->_icon->setVisible(pixmap.isNull() == false);
}

QLabel *AdmExtensionWidget::getIconLabel()
{
  return ui->_icon;
}
QLabel *AdmExtensionWidget::getExtenLabel()
{
  return ui->_exten;
}
QLabel *AdmExtensionWidget::getTextLabel()
{
  return ui->_desc;
}
void AdmExtensionWidget::dragEnterEvent(QDragEnterEvent *event)
{
  qDebug() << "AdmExtensionWidget::dragEnterEvent";
  //meDataAstCall
  const MimeDataAstCall *mime = qobject_cast<const MimeDataAstCall *>(event->mimeData());
  if(NULL != mime && mime->hasAdmCallWidget())
  {
    // Don't allow a call to transfer to channel in the same call
    AdmCallWidget *cw = mime->admCallWidget();
    bool found = false;
    cw->getChannelForDevice("SIP",this->getExten(),&found);
    if(!found)
    {
      // Accept the drop and change appearance
      emit sigDragEnterEvent(this,event);
      event->acceptProposedAction();
      qDebug() << QString("Original backgroundrole: %1")
                  .arg(backgroundRole());
      qDebug() << QString("Original foregroundrole: %2")
                  .arg(foregroundRole());

      setAutoFillBackground(true);
      setBackgroundRole(QPalette::Highlight);
      setForegroundRole(QPalette::HighlightedText);

      ui->_icon->setAutoFillBackground(true);
      ui->_icon->setBackgroundRole(QPalette::Button);
      ui->_icon->setForegroundRole(QPalette::ButtonText);
    } else {
      event->ignore();
    }
  } else {
    event->ignore();
  }
}
void AdmExtensionWidget::dragMoveEvent(QDragMoveEvent *event)
{
  qDebug() << "AdmExtensionWidget::dragMoveEvent";
  emit sigDragMoveEvent(this,event);
}

void AdmExtensionWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
  qDebug() << "AdmExtensionWidget::dragLeaveEvent";
  emit sigDragLeaveEvent(this, event);

  setAutoFillBackground(true);
  setBackgroundRole(QPalette::Button);
  setForegroundRole(QPalette::ButtonText);

  ui->_icon->setAutoFillBackground(true);
  ui->_icon->setBackgroundRole(QPalette::Button);
  ui->_icon->setForegroundRole(QPalette::ButtonText);

  event->accept();
}

void AdmExtensionWidget::dropEvent(QDropEvent *event)
{
  setAutoFillBackground(true);
  setBackgroundRole(QPalette::Button);
  setForegroundRole(QPalette::ButtonText);

  ui->_icon->setAutoFillBackground(true);
  ui->_icon->setBackgroundRole(QPalette::Button);
  ui->_icon->setForegroundRole(QPalette::ButtonText);

  const MimeDataAstCall *mime = qobject_cast<const MimeDataAstCall *>(event->mimeData());
  if(NULL != mime && mime->hasAdmCallWidget())
  {
    //AdmIconTextDrop *o = this;
    AdmCallWidget *cw = mime->admCallWidget();
    cw->sStartCallXfer(this);
    event->acceptProposedAction();
  }
}
