#include "admcallwidget.h"
#include "ui_admcallwidget.h"

#include "admstatic.h"

AdmCallWidget::AdmCallWidget(QString uuid, QFrame *parent) :
  QFrame(parent),
  ui(new Ui::AdmCallWidget)
{
	ui->setupUi(this);
  _uuid = new QString(uuid);
  _time = new QTime();
  _time->start();

  this->_channels = new QMap<QString,AstChannel*>();
  this->_channelWidgets = new QMap<QString,AdmChanWidget*>();

  connect(  AdmStatic::getInstance()->getTimer(),
                            SIGNAL(timeout()),  this, SLOT(sTickTock())         );
  connect(  ui->_tbXfer,    SIGNAL(clicked()),  this, SLOT(sStartCallXfer())    );
  connect(  ui->_tbPark,    SIGNAL(clicked()),  this, SLOT(sStartCallPark())    );
  connect(  ui->_tbHangup,  SIGNAL(clicked()),  this, SLOT(sStartCallHangup())  );
}

AdmCallWidget::~AdmCallWidget()
{
  emit destroying(this);
	delete ui;
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
}

void AdmCallWidget::sUpdateChannel(AstChannel *channel)
{
  qDebug() << tr("sUpdateChannel: Channel: '%1', uuid: '%2'")
              .arg(channel->getChannel())
              .arg(channel->getUuid())
  ;
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
    this->_channelWidgets->value(channel->getUuid())->sUpdateChannel();
  }
}

void AdmCallWidget::sHangupChannel(AstChannel *channel)
{
  Q_UNUSED(channel);
  bool isAllHungup = true;
  bool valid = false;
  uint cause;
  QMap<QString, AstChannel *>::iterator i;
  for(i = _channels->begin(); i != _channels->end(); ++i)
  {
    qDebug() << tr("UniqueId: '%1', Channel: '%2', Hangup: '%3'")
                .arg(i.value()->getUuid())
                .arg(i.value()->getChannel())
                .arg(i.value()->getHangupCauseStr())
                ;
    cause = i.value()->getHangupCauseNum(&valid);
    if(!valid)
    {
      isAllHungup = false;
      break;
    }
  }
  if(isAllHungup)
  {
    qDebug() << "Everyone hungup!";
    disconnect(AdmStatic::getInstance()
               ->getTimer(),  SIGNAL(timeout()),
               this,          SLOT(sTickTock())
    );
  }
}

void AdmCallWidget::sRemoveChannel(AstChannel *channel)
{
  qDebug() << "sRemoveChannel: " << channel->getUuid();
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

void AdmCallWidget::sStartCallXfer()
{
  emit callXfer(this,"730");
}
void AdmCallWidget::sStartCallXfer(AdmIconTextDrop *admIconTextDrop)
{
  emit callXfer(this,admIconTextDrop->getText());
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
    ChanPart *chanPart = i.value()->getChannelParts();
    if(chanPart->getType() == type && chanPart->getExten() == exten)
      return i.value();
  }
  *found = false;
  return NULL;
}
