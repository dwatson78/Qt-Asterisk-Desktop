#include "astcall.h"
#include <QSpacerItem>
#include <QToolButton>
#include "admstatic.h"


AstCall::AstCall(QString uuid, QWidget *parent) :
  QWidget(parent)
{
  _uuid = new QString(uuid);
  _elapsed = 0;
  _time = new QTime();
  _time->start();

  this->_channels = new QMap<QString,AstChannel*>();
  this->_channelWidgets = new QMap<QString,QLabel*>();

  this->_initWidgit();

  connect(AdmStatic::getInstance()
          ->getTimer(), SIGNAL(timeout()),
          this,         SLOT(sTickTock())
  );
}
AstCall::~AstCall()
{
  emit destroying(this);
}

void AstCall::_initWidgit()
{
  _lyt = new QGridLayout(this);
  _lytChannels = new QVBoxLayout();

  _lbl1 = new QLabel("Dial: ");
  setLayout(_lyt);
  _lyt->addWidget(_lbl1,0,0,1,1,Qt::AlignLeading|Qt::AlignVCenter);
  _lyt->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum),
                0,1,1,1,0);

  _lyt->addLayout(_lytChannels,1,0,1,2,0);

  //Set up the call buttons
  QHBoxLayout * _tbBox = new QHBoxLayout();
  _tbBox->setSpacing(2);

  // Set up Call Transfer
  QIcon iconCallXfer;
  iconCallXfer.addFile(QString::fromUtf8(":/icons/call-transfer.png"), QSize(), QIcon::Normal, QIcon::Off);
  QToolButton * _tbCallXfer = new QToolButton();
  _tbCallXfer->setObjectName(QString::fromUtf8("_tbCallXfer"));
  _tbCallXfer->setIcon(iconCallXfer);
  _tbCallXfer->setIconSize(QSize(24, 24));
  connect(_tbCallXfer,  SIGNAL(clicked()),
          this,             SLOT(sStartCallXfer())
  );
  _tbBox->addWidget(_tbCallXfer);

  // Set up Call Parking
  QIcon iconCallPark;
  iconCallPark.addFile(QString::fromUtf8(":/icons/call-park.png"), QSize(), QIcon::Normal, QIcon::Off);
  QToolButton * _tbCallPark = new QToolButton();
  _tbCallPark->setObjectName(QString::fromUtf8("_tbCallPark"));
  _tbCallPark->setIcon(iconCallPark);
  _tbCallPark->setIconSize(QSize(24, 24));
  connect(_tbCallPark,  SIGNAL(clicked()),
          this,             SLOT(sStartCallPark())
  );
  _tbBox->addWidget(_tbCallPark);

  // Set up Call Hangup
  QIcon iconCallEnd;
  iconCallEnd.addFile(QString::fromUtf8(":/icons/call-hangup.png"), QSize(), QIcon::Normal, QIcon::Off);
  QToolButton * _tbCallEnd = new QToolButton();
  _tbCallEnd->setObjectName(QString::fromUtf8("_tbCallEnd"));
  _tbCallEnd->setIcon(iconCallEnd);
  _tbCallEnd->setIconSize(QSize(24, 24));
  connect(_tbCallEnd,  SIGNAL(clicked()),
          this,             SLOT(sStartCallHangup())
  );
  _tbBox->addWidget(_tbCallEnd);

  // Finish setting up the call buttons
  _tbBox->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
  _lyt->addLayout(_tbBox,2,0,1,2,0);

  //Add a vertical spacer
  _lyt->addItem(new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding),
                3,0,1,2,0);
}

void AstCall::addChannel(AstChannel* channel)
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
  QLabel *lbl = new QLabel();
  lbl->setText(tr("%1 (%2)")
               .arg(channel->getCallIdStr())
               .arg(channel->getChannelStateStr())
  );
  this->_channelWidgets->insert(channel->getUuid(),lbl);
  _lytChannels->addWidget(lbl,0,0);

}

void AstCall::sUpdateChannel(AstChannel *channel)
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
            tr("%1 (%2)%3%4")
            .arg(channel->getCallIdStr())
            .arg(channel->getChannelStateStr())
            .arg(channel->getIsMusicOn()  ? " Music"  : "")
            .arg(channel->getIsParked()   ? QString(" Parked on %1").arg(channel->getParkedExten(new bool)) : "")
      );
    } else {
      this->_channelWidgets->value(channel->getUuid())->setText(
            tr("%1 (Hangup %2)%3%4")
            .arg(channel->getCallIdStr())
            .arg(channel->getHangupCauseStr())
            .arg(channel->getIsMusicOn()  ? " Music"  : "")
            .arg(channel->getIsParked()   ? QString(" Parked on %1").arg(channel->getParkedExten(new bool)) : "")
      );
    }
  }
}

void AstCall::sHangupChannel(AstChannel *channel)
{
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

void AstCall::sRemoveChannel(AstChannel *channel)
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

void AstCall::sTickTock()
{
  _lbl1->setText(tr("Dial: Elapsed(%1)").arg(AdmStatic::elapsedTimeToString(_time)));
}

void AstCall::sStartCallXfer()
{
  emit callXfer(this,"730");
}
void AstCall::sStartCallPark()
{
  emit callPark(this);
}
void AstCall::sStartCallHangup()
{
  emit callHangup(this);
}

AstChannel * AstCall::getChannelForDevice(QString type, QString exten, bool *found)
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

