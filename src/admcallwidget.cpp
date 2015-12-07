#include "admcallwidget.h"
#include "ui_admcallwidget.h"

#include "admstatic.h"

#include "admintercomvideodialog.h"

AdmCallWidget::AdmCallWidget(QString uuid, QFrame *parent) :
  QFrame(parent),
  ui(new Ui::AdmCallWidget)
{
	ui->setupUi(this);
  _uuid = uuid;
  _time = new QTime();
  _time->start();

  this->_channels = new QMap<QString,AstChannel*>();
  this->_channelWidgets = new QMap<QString,AdmChanWidget*>();

  connect(  AdmStatic::getInstance()->getTimer(),
                            SIGNAL(timeout()),  this, SLOT(sTickTock())         );
  connect(  ui->_tbPark,    SIGNAL(clicked()),  this, SLOT(sStartCallPark())    );
  connect(  ui->_tbHangup1,  SIGNAL(clicked()),  this, SLOT(sStartCallHangup())  );
  connect(  ui->_tbHangup2,  SIGNAL(clicked()),  this, SLOT(sStartCallHangup())  );
  ui->_stackedButtons->setCurrentIndex(0);
}

AdmCallWidget::~AdmCallWidget()
{
  qDebug() << "AdmCallWidget::~AdmCallWidget()";
  emit destroying(this);
	delete ui;
  delete _time;
  delete _channels;
  delete _channelWidgets;
}

void AdmCallWidget::addChannel(AstChannel* channel)
{
  if(this->_channels->contains(channel->getUuid()))
  {
    this->_channels->remove(channel->getUuid());
  }
  this->_channels->insert(channel->getUuid(),channel);
  connect(channel,  SIGNAL(updated(AstChannel *)),
          this,     SLOT(sUpdateChannel(AstChannel *))
  );
  connect(channel,  SIGNAL(hangup(AstChannel*)),
          this,     SLOT(sHangupChannel(AstChannel *))
  );
  connect(channel,  SIGNAL(destroying(AstChannel *)),
          this,     SLOT(sRemoveChannel(AstChannel *))
  );
  if(this->_channelWidgets->contains(channel->getUuid()))
  {
    this->_channelWidgets->value(channel->getUuid())->deleteLater();
    this->_channelWidgets->remove(channel->getUuid());
  }
  AdmChanWidget *chanWidget = new AdmChanWidget(channel);
  this->_channelWidgets->insert(channel->getUuid(),chanWidget);
  ui->_layoutChans->addWidget(chanWidget);
  this->sUpdateChannel(channel);
}

void AdmCallWidget::sUpdateChannel(AstChannel *channel)
{
  if(this->_channelWidgets->contains(channel->getUuid()))
  {
    bool valid = false;
    channel->getHangupCauseNum(&valid);
    if(!valid)
    {
      this->_channelWidgets->value(channel->getUuid())->setText(
            tr("%1 (%2)%3")
            .arg(channel->getCallIdStr())
            .arg(channel->getChannelStateStr())
            .arg(channel->getIsParked()   ? QString(" Parked on %1").arg(channel->getParkedExten(new bool)) : "")
      );
    } else {
      this->_channelWidgets->value(channel->getUuid())->setText(
            tr("%1 (Hangup %2)%3")
            .arg(channel->getCallIdStr())
            .arg(channel->getHangupCauseStr())
            .arg(channel->getIsParked()   ? QString(" Parked on %1").arg(channel->getParkedExten(new bool)) : "")
      );
    }
    if(ui->_stackedButtons->currentIndex() != 1)
    {
      bool ok = false;
      uint chanStateVal = channel->getChannelState(&ok);
      if(ok)
      {
        if(chanStateVal == AsteriskManager::ChanStateUp)
        {
          ui->_stackedButtons->setCurrentIndex(1);
        }
      }
    }
    this->_channelWidgets->value(channel->getUuid())->sUpdateChannel();
  }
  // Check to see if this user is on the phone with the door intercom
  bool isMyDevice = false;
  bool isDoorIntercom = false;
  AstChannel *chanDoor = NULL;
  QMap<QString, AstChannel *>::iterator i;
  for(i = _channels->begin(); i != _channels->end(); ++i)
  {
    AstChanParts *chanPart = i.value()->getChannelParts();
    if(chanPart->isMyDevice())
    {
      isMyDevice = true;
    }
    else if(chanPart->getType() == "SIP" && chanPart->getExten() == "6001")
    {
      isDoorIntercom = true;
      chanDoor = i.value();
    }
    if(isMyDevice && isDoorIntercom)
    {
      AdmIntercomVideoDialog *dlg = new AdmIntercomVideoDialog(0,chanDoor,QUrl("rtsp://10.121.212.23:554/live.sdp"), "46");
      //dlg.setModal(false);
      dlg->show();
      dlg->raise();
      dlg->activateWindow();
      //dlg.exec();
      break;
    }
  }
}

void AdmCallWidget::sHangupChannel(AstChannel *channel)
{
  Q_UNUSED(channel);

  ui->_stackedButtons->setCurrentIndex(2);

  bool isAllHungup = true;
  bool valid = false;
  QMap<QString, AstChannel *>::iterator i;
  for(i = _channels->begin(); i != _channels->end(); ++i)
  {
    i.value()->getHangupCauseNum(&valid);
    if(!valid)
    {
      isAllHungup = false;
      break;
    }
  }
  if(isAllHungup)
  {
    disconnect(AdmStatic::getInstance()
               ->getTimer(),  SIGNAL(timeout()),
               this,          SLOT(sTickTock())
    );
  }
}

void AdmCallWidget::sRemoveChannel(AstChannel *channel)
{
  ui->_stackedButtons->setCurrentIndex(2);
  disconnect(channel,  SIGNAL(updated(AstChannel *)),
          this,     SLOT(sUpdateChannel(AstChannel *))
  );
  disconnect(channel,  SIGNAL(hangup(AstChannel*)),
          this,     SLOT(sHangupChannel(AstChannel *))
  );
  disconnect(channel,  SIGNAL(destroying(AstChannel *)),
          this,     SLOT(sRemoveChannel(AstChannel *))
  );
  if(this->_channels->contains(channel->getUuid()))
    this->_channels->remove(channel->getUuid());
  if(this->_channelWidgets->contains(channel->getUuid()))
  {
    this->_channelWidgets->value(channel->getUuid())->deleteLater();
    this->_channelWidgets->remove(channel->getUuid());
  }
  if(this->_channels->count() == 0)
    this->deleteLater();
}

void AdmCallWidget::sTickTock()
{
  ui->_status3->setText(QString("Dial: Elapsed(%1)")
                        .arg(AdmStatic::elapsedTimeToString(_time)));
}

void AdmCallWidget::sStartCallXfer(AdmExtensionWidget *widget)
{
  emit callXfer(this,widget->getExten());
}

void AdmCallWidget::sStartCallPark()
{
  emit callPark(this);
}
void AdmCallWidget::sStartCallHangup()
{
  emit callHangup(this);
}

AstChannel * AdmCallWidget::getChannelForDevice(QString type, QString exten, bool *found)
{
  *found = true;
  QMap<QString, AstChannel *>::iterator i;
  for(i = _channels->begin(); i != _channels->end(); ++i)
  {
    AstChanParts *chanPart = i.value()->getChannelParts();
    if(chanPart->getType() == type && chanPart->getExten() == exten)
      return i.value();
  }
  *found = false;
  return NULL;
}
