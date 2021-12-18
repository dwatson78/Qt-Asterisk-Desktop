#include "admextensionwidget.h"
#include "ui_admextensionwidget.h"

#include "admstatic.h"

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
  _statusNum = AsteriskManager::NotInUse;
  _isDndOn = false;
  _sipPeer = NULL;
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
  emit sigDragMoveEvent(this,event);
}

void AdmExtensionWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
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

void AdmExtensionWidget::setSipPeer(AstSipPeer *peer)
{
  // If it's the same peer, let's get out of here.
  if(peer == _sipPeer)
    return;

  if(NULL != _sipPeer)
  {
    disconnect( _sipPeer, SIGNAL(destroying(AstSipPeer*)),
                this,     SLOT(sSipPeerDestroying(AstSipPeer*)));
    disconnect( _sipPeer, SIGNAL(sUpdated(AstSipPeer*)),
                this,     SLOT(sSipPeerUpdated(AstSipPeer*)));
    disconnect( _sipPeer, SIGNAL(sigExtensionStatusEvent(AstSipPeer*,QVariantMap)),
                this,     SLOT(sSipExtensionStatusEvent(AstSipPeer*,QVariantMap)));
    disconnect( _sipPeer, SIGNAL(sigDndStatusEvent(AstSipPeer*,QVariantMap,bool)),
                this,     SLOT(sSipExtensionDndStatusEvent(AstSipPeer*,QVariantMap,bool)));
    _sipPeer = NULL;
  }

  if(NULL != peer)
  {
    _sipPeer = peer;
    connect(_sipPeer, SIGNAL(destroying(AstSipPeer*)),
            this,     SLOT(sSipPeerDestroying(AstSipPeer*)));
    connect(_sipPeer, SIGNAL(sUpdated(AstSipPeer*)),
            this,     SLOT(sSipPeerUpdated(AstSipPeer*)));
    connect(_sipPeer, SIGNAL(sigExtensionStatusEvent(AstSipPeer*,QVariantMap)),
            this,     SLOT(sSipExtensionStatusEvent(AstSipPeer*,QVariantMap)));
    connect(_sipPeer, SIGNAL(sigDndStatusEvent(AstSipPeer*,QVariantMap,bool)),
            this,     SLOT(sSipExtensionDndStatusEvent(AstSipPeer*,QVariantMap,bool)));
    _updateStatusFromSipPeerStatus(peer->getPeerStatus());
  }
}

AstSipPeer *AdmExtensionWidget::getSipPeer()
{
  return _sipPeer;
}

void AdmExtensionWidget::sSipPeerDestroying(AstSipPeer *peer)
{
  Q_UNUSED(peer);
  disconnect(_sipPeer, SIGNAL(destroying(AstSipPeer*)),
          this,     SLOT(sSipPeerDestroying(AstSipPeer*)));
  disconnect(_sipPeer, SIGNAL(sUpdated(AstSipPeer*)),
          this,     SLOT(sSipPeerUpdated(AstSipPeer*)));
  disconnect(_sipPeer, SIGNAL(sigExtensionStatusEvent(AstSipPeer*,QVariantMap)),
          this,     SLOT(sSipExtensionStatusEvent(AstSipPeer*,QVariantMap)));
  disconnect(_sipPeer, SIGNAL(sigDndStatusEvent(AstSipPeer*,QVariantMap,bool)),
          this,     SLOT(sSipExtensionDndStatusEvent(AstSipPeer*,QVariantMap,bool)));
  ui->_desc->setText("N/A");
  ui->_exten->setText("N/A");
  setPixmap(QPixmap(":/icons/status-unavail.png"));
  _sipPeer = NULL;
}

void AdmExtensionWidget::sSipPeerUpdated(AstSipPeer *peer)
{
  ui->_desc->setText(peer->getDescription());
  _updateStatusFromSipPeerStatus(peer->getPeerStatus());
}

void AdmExtensionWidget::_updateStatusFromSipPeerStatus(const QString &status)
{
  if(   status.startsWith("OK")
     || status == "Registered"
     || status == "Reachable")
    _statusNum = _sipPeer->getExtStatus();
  else
    _statusNum = AsteriskManager::Unavailable;

  _updateStatusIcon();
}

void AdmExtensionWidget::sExtensionStatusEvent(const QVariantMap &event)
{
  if(event.contains("Status"))
  {
    uint statusNum = event.value("Status").toUInt();
    _statusNum = statusNum;
    _updateStatusIcon();
  }
}

void AdmExtensionWidget::sExtensionDndStatusEvent(const QVariantMap &event)
{
  if(event.contains("Status"))
  {
    uint statusNum = event.value("Status").toUInt();
    AsteriskManager::ExtStatuses statuses(statusNum);
    _isDndOn = !statuses.testFlag(AsteriskManager::NotInUse);
    _updateStatusIcon();
  }
}

void AdmExtensionWidget::sSipExtensionStatusEvent(AstSipPeer *peer, const QVariantMap &event)
{
  if(   NULL != _sipPeer
        && NULL != peer
        && peer == _sipPeer
    )
  {
    this->sExtensionStatusEvent(event);
  }
}

void AdmExtensionWidget::sSipExtensionDndStatusEvent(AstSipPeer *peer, const QVariantMap &event, bool isDndOn)
{
  if(   NULL != _sipPeer
     && NULL != peer
     && peer == _sipPeer)
  {
    //Status already checked by the AstSipPeer class before emitting signal
    if(event.count() > 0)
      sExtensionDndStatusEvent(event);
    else
    {
      _isDndOn = isDndOn;
      _updateStatusIcon();
    }

    if(_isDndOn != isDndOn)
    {
      qCritical() << QString("AdmExtensionWidget::sSipExtensionDndStatusEvent: local value _isDndOn does not match this function's parameter isDndOn: _isDndOn: %1, isDndOn: %1")
                     .arg(_isDndOn)
                     .arg(isDndOn);
    }
  }
}

void AdmExtensionWidget::_updateStatusIcon()
{
  AsteriskManager::ExtStatuses statuses(_statusNum);
  if(   statuses.testFlag(AsteriskManager::Deactivated)
     || statuses.testFlag(AsteriskManager::Removed)
     || statuses.testFlag(AsteriskManager::Unavailable))
  {
    setPixmap(QPixmap(":/icons/status-unavail.png"));
  } else if (_isDndOn) {
    setPixmap(QPixmap(":/icons/status-dnd.png"));
  } else {
    if(statuses.testFlag(AsteriskManager::NotInUse))
    {
      setPixmap(QPixmap(":/icons/status-avail.png"));
    } else if(statuses.testFlag(AsteriskManager::InUse)
            || statuses.testFlag(AsteriskManager::Busy))
    {
      setPixmap(QPixmap(":/icons/status-busy.png"));
    }
    if(statuses.testFlag(AsteriskManager::Ringing))
    {
      setPixmap(QPixmap(":/icons/status-ring.png"));
    }
    if(statuses.testFlag(AsteriskManager::OnHold))
    {
      setPixmap(QPixmap(":/icons/status-onhold.png"));
    }
  }
}
