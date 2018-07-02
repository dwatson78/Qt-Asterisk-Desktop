#include "confbridgeuser.h"

#include <QDebug>
#include <QMenu>

ConfBridgeUser::ConfBridgeUser(QWidget *parent, AsteriskManager* ami, QVariantMap event):
  QWidget(parent)
{
  /*
      arg2:  "CallerIDname" :  QVariant(QString, "Daniel W Work PC")
      arg2:  "CallerIDnum" :  QVariant(uint, 4004)
      arg2:  "Channel" :  QVariant(QString, "SIP/4004-00000012")
      arg2:  "Conference" :  QVariant(uint, 1080)
      arg2:  "Event" :  QVariant(QString, "ConfbridgeJoin")
      arg2:  "Privilege" :  QVariant(QString, "call,all")
      arg2:  "Uniqueid" :  QVariant(QString, "1434151700.44")
  */

  if(_ami == NULL)
    deleteLater();
  else
    this->_ami = ami;

  if(event.contains("Conference"))
    this->_conference = event.value("Conference").toUInt();
  else
    deleteLater();
  if(event.contains("Uniqueid"))
    this->_uuid = event.value("Uniqueid").toString();
  else
    deleteLater();
  if(event.contains("Channel"))
    this->_channel = event.value("Channel").toString();
  else
    deleteLater();

  talkingWhileMuted = false;
  muted = false;
  talking = false;

  setObjectName(tr("cbu_%1_%2")
                .arg(QString::number(this->_conference))
                .arg(this->_uuid));

  QString lblStr;
  lblStr = tr("%1 <%2>")
      .arg(event.contains("CallerIDName") ? event.value("CallerIDName").toString() : "UNKNOWN")
      .arg(event.contains("CallerIDNum") ? QString::number(event.value("CallerIDNum").toUInt()) : "");
  _lbl = new QLabel(lblStr);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(contextMenuRequested(const QPoint&))
  );

  _lyt = new QHBoxLayout(this);
  _lyt->setMargin(0);
  _lyt->addWidget(_lbl);
}

void ConfBridgeUser::setTalkingStatus(bool talking)
{
  this->talking = talking;
  if(talking)
  {
    _lbl->setStyleSheet("QLabel { color: green; }");
  } else {
    _lbl->setStyleSheet("QLabel { color: black; }");
  }
}

void ConfBridgeUser::contextMenuRequested(const QPoint& point)
{
  QPoint globalPos = _lbl->mapToGlobal(point);
  QMenu* menu = new QMenu(this);
  QAction* act1 = menu->addAction("Mute User");
  connect(act1, SIGNAL(triggered()),
          this, SLOT(mute())
  );
  QAction* act2 = menu->addAction("Unmute User");
  connect(act2, SIGNAL(triggered()),
          this, SLOT(unmute())
  );
  QAction* act3 = menu->addAction("Kick User");
  connect(act3, SIGNAL(triggered()),
          this, SLOT(kick())
  );
  menu->popup(globalPos);
}


void ConfBridgeUser::mute()
{
  if(this->talking)
  {
    talkingWhileMuted = true;
    this->setTalkingStatus(false);
  }
  QString result = _ami->actionConfbridgeMute(this->_conference, this->_channel);
}

void ConfBridgeUser::unmute()
{
  if(this->talkingWhileMuted)
  {
    talkingWhileMuted = true;
    this->setTalkingStatus(true);
  }
  _ami->actionConfbridgeUnmute(this->_conference, this->_channel);
}
void ConfBridgeUser::kick()
{
  _ami->actionConfbridgeKick(this->_conference, this->_channel);
}
