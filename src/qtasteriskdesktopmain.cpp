#include "qtasteriskdesktopmain.h"
#include "ui_qtasteriskdesktopmain.h"

#include "qtasteriskdesktopprefs.h"

#include "asteriskmanager.h"
#include "confbridgeuser.h"

#include <QDebug>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <QStringList>

QtAsteriskDesktopMain::QtAsteriskDesktopMain(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::QtAsteriskDesktopMain)
{
  ui->setupUi(this);
  _chanMap = new QMap<QString, AstChannel*>();
  _parkedMap = new QMap<QString, AstParkedCall*>();
  _callMap = new QMap<QString, AstCall*>();
  _loginActionId = QString();
  _statusIcon = new StatusIcon(this);
  ui->statusBar->addPermanentWidget(_statusIcon);

  /*Connect the Menu Actions*/
  connect(ui->actionPreferences, SIGNAL(triggered()), 
          this,                  SLOT(sPreferences())
  );

  _ami = new AsteriskManager(this);
  connect( _ami, SIGNAL(connected()),
           this, SLOT(asteriskConnected())
  );
  connect(_ami, SIGNAL(disconnected()),
          this, SLOT(asteriskDisconnected())
  );
  connect(_ami, SIGNAL(responseSent(AsteriskManager::Response,QVariantMap,QString)),
          this, SLOT(asteriskResponseSent(AsteriskManager::Response,QVariantMap,QString))
  );
  connect(_ami, SIGNAL(eventGenerated(AsteriskManager::Event,QVariantMap)),
          this, SLOT(asteriskEventGenerated(AsteriskManager::Event,QVariantMap))
  );
  QSettings settings;
  settings.beginGroup("AMI");
  if(settings.contains(AmiPref::getName(AmiPref::host)) && settings.contains(AmiPref::getName(AmiPref::port)))
  {
    bool *valid = new bool(false);
    
    QString host = settings.value(AmiPref::getName(AmiPref::host)).toString();
    uint port = settings.value(AmiPref::getName(AmiPref::port)).toUInt(valid);
    if(valid)
      _ami->connectToHost(host, port);
    else
      asteriskDisconnected();
  }
  else
    asteriskDisconnected();
  settings.endGroup();
  if(settings.contains("DEVICES/default"))
  {
    QString dvc = settings.value("DEVICES/default").toString();
    QStringList parts = dvc.split("/");
    bool *valid = new bool(false);
    uint dvcExt = parts.at(1).toUInt(valid);
    if(valid)
      ui->_dvcExt->setText(QString::number(dvcExt));
  }
}

QtAsteriskDesktopMain::~QtAsteriskDesktopMain()
{
  delete ui;
}

void QtAsteriskDesktopMain::asteriskConnected()
{
  qDebug() << "Connected!";
  _statusIcon->setConnected(true);
  QSettings settings;
  settings.beginGroup("AMI");
  if(settings.contains(AmiPref::getName(AmiPref::userName)) && settings.contains(AmiPref::getName(AmiPref::secPass)))
    _loginActionId = _ami->actionLogin(settings.value(AmiPref::getName(AmiPref::userName)).toString(),settings.value(AmiPref::getName(AmiPref::secPass)).toString());
  else
    _ami->disconnectFromHost();
}

void QtAsteriskDesktopMain::asteriskConnected(QString arg1)
{
  qDebug() << "Connected!";
  qDebug() << "arg1: " << arg1;
  _statusIcon->setConnected(true);
}

void QtAsteriskDesktopMain::asteriskDisconnected()
{
  qDebug() << "Disonnected!";
  // _statusIcon->setConnected(false);
}

void QtAsteriskDesktopMain::asteriskResponseSent(AsteriskManager::Response arg1, QVariantMap arg2, QString arg3)
{
  QString response;
  switch(arg1)
  {
    case (AsteriskManager::Success):
      response = "Success";
      break;
    case (AsteriskManager::Error):
      response = "Error";
      break;
    case (AsteriskManager::Follows):
      response = "Follows";
      break;
  }
  qDebug() << "arg1: " << response;
  for(QVariantMap::const_iterator iter = arg2.begin();
      iter != arg2.end();
      ++iter)
  {
    qDebug() << "arg2: " << iter.key() << ": " << iter.value();
  }
  qDebug() << "arg3: " << arg3;
  if(arg3 == _loginActionId)
  {
    qDebug() << "This is our login result!!!!\n\n";
    _statusIcon->setLoggedIn(arg1 == AsteriskManager::Success, arg2.value("Message").toString());
    ui->_buttonDial->setEnabled(arg1 == AsteriskManager::Success);
    if(arg1 == AsteriskManager::Success)
    {
      connect(ui->_buttonDial, SIGNAL(clicked()),
              this,            SLOT(sDial())
      );
    } else {
      disconnect(ui->_buttonDial, SIGNAL(clicked()),
                 this,            SLOT(sDial())
      );
    }
  }
}

void QtAsteriskDesktopMain::asteriskEventGenerated(AsteriskManager::Event arg1, QVariantMap arg2)
{
  //Skip the following events
  switch(arg1)
  {
    case AsteriskManager::VarSet:
    case AsteriskManager::Newexten:
      return;
    default:
      break;
  }

  switch(arg1)
  {
    case AsteriskManager::ConfbridgeEnd:
    case AsteriskManager::ConfbridgeJoin:
    case AsteriskManager::ConfbridgeLeave:
    case AsteriskManager::ConfbridgeStart:
    case AsteriskManager::ConfbridgeTalking:
      //qDebug() << "Begin Event Info";
      for(QVariantMap::const_iterator iter = arg2.begin();
          iter != arg2.end();
          ++iter)
      {
        //qDebug() << "arg2: " << iter.key() << ": " << iter.value();
      }
      //qDebug() << "End Event Info\n\n";
      break;
    default:
      break;
  }

  bool catchMe = false;
  for(QVariantMap::const_iterator iter = arg2.begin();
      iter != arg2.end();
      ++iter)
  {
    if( iter.value().toString().contains("4004")
        || iter.value().toString().contains("4005")
      )
    {
      catchMe = true;
      break;
    }
  }
  if(catchMe)
  {
    const QMetaObject &mo = AsteriskManager::staticMetaObject;
    int index = mo.indexOfEnumerator("Event");
    QMetaEnum me = mo.enumerator(index);
    qDebug() << "\ncatchMeIfYouCan: Begin Caught Event Info";
    qDebug() << "catchMeIfYouCan: arg1: Event: " << me.valueToKey(arg1);
    for(QVariantMap::const_iterator iter = arg2.begin();
        iter != arg2.end();
        ++iter)
    {

      if(QString(iter.key()) != "ChanVars")
      {
        qDebug() << "catchMeIfYouCan: arg2: " << iter.key() << ": " << iter.value();
      } else {
        QVariantMap chanVars = iter.value().toMap();
        for(QVariantMap::const_iterator iter2 = chanVars.begin();
            iter2 != chanVars.end();
            ++iter2)
        {
          QVariantMap chanVarVals = iter2.value().toMap();
          for(QVariantMap::const_iterator iter3 = chanVarVals.begin();
              iter3 != chanVarVals.end();
              ++iter3)
          {
            qDebug() << "catchMeIfYouCan: ChanVar: "
                     << iter2.key()
                     << ": "
                     << iter3.key()
                     << ": "
                     << iter3.value()
            ;
          }
        }
        if(arg2.contains("Channel"))
        {
          bool found = false;

          QVariant chanVarUuid = AstChannel::getChanVar(
                arg2,
                arg2.value("Channel").toString(),
                "CHANNEL(uniqueid)",
                &found
                );
          if(found)
          {
            qDebug() << "Test Static: " << chanVarUuid.toString();
          }
        }
      }
    }
    if(arg1 == AsteriskManager::ExtensionStatus)
    {
      uint status = arg2.value("Status").toUInt();
      AsteriskManager::ExtStatuses f1(status);
      sSetExtStatus(arg2.value("Exten").toUInt(), f1);

    }
    qDebug() << "catchMeIfYouCan: End Caught Event Info\n\n";
  }

  switch(arg1)
  {
    case AsteriskManager::ConfbridgeStart:
      this->showConferenceRoom(arg2);
      break;
    case AsteriskManager::ConfbridgeEnd:
      this->hideConferenceRoom(arg2);
      break;
    case AsteriskManager::ConfbridgeJoin:
      this->addConfBridgeUser(arg2);
      break;
    case AsteriskManager::ConfbridgeLeave:
      this->deleteConfBridgeUser(arg2);
      break;
    case AsteriskManager::ConfbridgeTalking:
      this->confBridgeTalking(arg2);
      break;
    case AsteriskManager::Newchannel:
    {
      AstChannel *chan = new AstChannel(arg2, this);
      connect(chan, SIGNAL(destroying(AstChannel*)),
              this, SLOT(sDestroyingChannel(AstChannel*))
      );
      _chanMap->insert(chan->getUuid(), chan);
      break;
    }
    case AsteriskManager::Masquerade:
    {
      /*
        catchMeIfYouCan: Begin Caught Event Info
        catchMeIfYouCan: arg1: Event:  Masquerade
        catchMeIfYouCan: ChanVar:  "Parked/SIP/4005-0000020c" :  "CHANNEL(uniqueid)" :  QVariant(QString, "1434654886.624")
        catchMeIfYouCan: ChanVar:  "SIP/4005-0000020c" :  "CHANNEL(uniqueid)" :  QVariant(QString, "1434654879.623")
        catchMeIfYouCan: arg2:  "Clone" :  QVariant(QString, "SIP/4005-0000020c")
        catchMeIfYouCan: arg2:  "CloneState" :  QVariant(QString, "Up")
        catchMeIfYouCan: arg2:  "Event" :  QVariant(QString, "Masquerade")
        catchMeIfYouCan: arg2:  "Original" :  QVariant(QString, "Parked/SIP/4005-0000020c")
        catchMeIfYouCan: arg2:  "OriginalState" :  QVariant(QString, "Down")
        catchMeIfYouCan: arg2:  "Privilege" :  QVariant(QString, "call,all")
        catchMeIfYouCan: End Caught Event Info
      */
      bool origFound = false;
      QVariant vOrigUuid = AstChannel::getChanVar(
            arg2,
            arg2.value("Original").toString(),
            "CHANNEL(uniqueid)",
            &origFound
      );
      if(origFound && _chanMap->contains(vOrigUuid.toString()))
      {
        bool cloneFound = false;
        QVariant vCloneUuid = AstChannel::getChanVar(
              arg2,
              arg2.value("Clone").toString(),
              "CHANNEL(uniqueid)",
              &cloneFound
        );
        if(cloneFound && _chanMap->contains(vCloneUuid.toString()))
        {
          AstChannel *origChan = _chanMap->value(vOrigUuid.toString());
          AstChannel *cloneChan = _chanMap->value(vCloneUuid.toString());
          origChan->sMasqueradeChannel(arg2, cloneChan);
        }
      }
      break;
    }
    case AsteriskManager::ParkedCall:
    {
      QString uuid;
      if(arg2.contains("Uniqueid"))
        uuid = arg2.value("Uniqueid",QString()).toString();
      else if(arg2.contains("UniqueId"))
        uuid = arg2.value("UniqueId",QString()).toString();
      else if(arg2.contains("UniqueID"))
        uuid = arg2.value("UniqueID",QString()).toString();
      else
        uuid = QString();
      QString fromChannelName;
      if(arg2.contains("From"))
        fromChannelName = arg2.value("From").toString();
      if(uuid != QString() && fromChannelName != QString() && _chanMap->contains(uuid))
      {
        if(!_parkedMap->contains(uuid))
        {
          AstChannel *chanParked = _chanMap->value(uuid);
          AstChannel *chanFrom = NULL;
          QMap<QString, AstChannel *>::iterator i;
          for(i = _chanMap->begin(); i != _chanMap->end(); ++i)
          {
            if(i.key() == uuid)
              continue;
            if(i.value()->getChannel() == fromChannelName)
            {
              chanFrom = i.value();
              break;
            }
          }
          AstParkedCall *parkedCall = new AstParkedCall(chanParked, chanFrom, arg2);
          connect(parkedCall, SIGNAL(destroying(AstParkedCall*)),
                  this,       SLOT(sDestroyingParkedCalled(AstParkedCall*))
          );
          connect(parkedCall, SIGNAL(pickUpParkedCall(AstParkedCall*)),
                  this,       SLOT(sPickUpParkedCall(AstParkedCall*))
          );
          _parkedMap->insert(uuid,parkedCall);
          ui->_layoutParked->addWidget(parkedCall);
        }
      }
    }
    case AsteriskManager::UnParkedCall:
    case AsteriskManager::ParkedCallGiveUp:
    case AsteriskManager::ParkedCallTimeOut:
    {
      if(arg1 != AsteriskManager::ParkedCall)
      {
        QString uuid;
        if(arg2.contains("Uniqueid"))
          uuid = arg2.value("Uniqueid",QString()).toString();
        else if(arg2.contains("UniqueId"))
          uuid = arg2.value("UniqueId",QString()).toString();
        else if(arg2.contains("UniqueID"))
          uuid = arg2.value("UniqueID",QString()).toString();
        else
          uuid = QString();
        if(uuid != QString() && _parkedMap->contains(uuid))
        {
          _parkedMap->value(uuid)->sParkedCallEvent(arg1, arg2);
        }
      }
      break;
    }
    case AsteriskManager::Hangup:
    case AsteriskManager::Newstate:
    case AsteriskManager::NewCallerid:
    case AsteriskManager::Rename:
    case AsteriskManager::MusicOnHold:
    {
      //Get the Uniqueid
      QString uuid;
      if(arg2.contains("Uniqueid"))
        uuid = arg2.value("Uniqueid",QString()).toString();
      else if(arg2.contains("UniqueId"))
        uuid = arg2.value("UniqueId",QString()).toString();
      else if(arg2.contains("UniqueID"))
        uuid = arg2.value("UniqueID",QString()).toString();
      else
        uuid = QString();
      if(uuid != QString() && _chanMap->contains(uuid))
      {
        AstChannel *chan = _chanMap->value(uuid,0);
        chan->sChannelEvent(arg1, arg2);
      } else {
        qDebug() << "Could not match event to uuid: " << uuid << arg2;
      }
      break;
    }
    case AsteriskManager::Bridge:
    {
      if(arg2.value("Bridgestate").toString() == "Link")
      {
        QString uuid1,uuid2;
        uuid1 = arg2.value("Uniqueid1").toString();
        uuid2 = arg2.value("Uniqueid2").toString();

        // Call handling
        if(!_callMap->contains(uuid1))
        {
          AstCall *call = new AstCall(uuid1);
          connect(call, SIGNAL(callXfer(AstCall*,QString)),
                  this, SLOT(sCallXfer(AstCall*,QString))
          );
          connect(call, SIGNAL(callPark(AstCall*)),
                  this, SLOT(sCallPark(AstCall*))
          );
          connect(call, SIGNAL(callHangup(AstCall*)),
                  this, SLOT(sCallHangup(AstCall*))
          );
          connect(call, SIGNAL(destroying(AstCall*)),
                  this, SLOT(sDestroyingCall(AstCall*))
          );

          if(_chanMap->contains(uuid1))
            call->addChannel(_chanMap->value(uuid1));

          if(_chanMap->contains(uuid2))
            call->addChannel(_chanMap->value(uuid2));

          _callMap->insert(uuid1,call);

          ui->_layoutMyDevice->addWidget(call,2,0,1,1,0);
        }
        // Parked call handling
        if(!uuid1.isNull() && !uuid2.isNull())
        {
          if(_parkedMap->contains(uuid1) && _chanMap->contains(uuid2))
          {
            _parkedMap->value(uuid1)->sParkedCallEvent(arg1,arg2,_chanMap->value(uuid2));
          }
        }
      }
      break;
    }
    case AsteriskManager::Dial:
    {
      if(arg2.value("SubEvent").toString() == "Begin")
      {
        QString uuid = arg2.value("UniqueID").toString();
        if(_chanMap->contains(uuid))
        {
          AstCall *call = new AstCall(uuid);
          connect(call, SIGNAL(callXfer(AstCall*,QString)),
                  this, SLOT(sCallXfer(AstCall*,QString))
          );
          connect(call, SIGNAL(callPark(AstCall*)),
                  this, SLOT(sCallPark(AstCall*))
          );
          connect(call, SIGNAL(callHangup(AstCall*)),
                  this, SLOT(sCallHangup(AstCall*))
          );
          connect(call, SIGNAL(destroying(AstCall*)),
                  this, SLOT(sDestroyingCall(AstCall*))
          );
          call->addChannel(_chanMap->value(uuid));

          QString destUuid = arg2.value("DestUniqueID").toString();
          if(_chanMap->contains(destUuid))
            call->addChannel(_chanMap->value(destUuid));

          _callMap->insert(uuid,call);

          ui->_layoutMyDevice->addWidget(call,2,0,1,1,0);
        }
      }
      break;
    }
    default:
      break;
  }
}

void QtAsteriskDesktopMain::showConferenceRoom(QVariantMap arg1)
{
  QGroupBox *groupBox = new QGroupBox(ui->_tab2);
  QString conference = QString::number(arg1.value("Conference").toUInt());
  groupBox->setObjectName(tr("_confBridge%1").arg(conference));
  groupBox->setStyleSheet(QString::fromUtf8("border-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"));
  groupBox->setFlat(false);
  groupBox->setCheckable(false);
  groupBox->setTitle(tr("Conference Room: %1").arg(conference));
  QGridLayout *layout = new QGridLayout(ui->_tab2);
  groupBox->setLayout(layout);
  ui->_layoutConfRoom->addWidget(groupBox,0,0,1,1,Qt::AlignLeft | Qt::AlignTop);
}

void QtAsteriskDesktopMain::hideConferenceRoom(QVariantMap arg1)
{
  QString conference = QString::number(arg1.value("Conference").toUInt());
  QGroupBox* gp =  findChild<QGroupBox*>(tr("_confBridge%1").arg(conference));
  if(gp != NULL)
  {
    ui->_layoutConfRoom->removeWidget(gp);
    gp->deleteLater();
  }
}

void QtAsteriskDesktopMain::addConfBridgeUser(QVariantMap arg1)
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
  QString conference = QString::number(arg1.value("Conference").toUInt());
  QGroupBox* gp =  findChild<QGroupBox*>(tr("_confBridge%1").arg(conference));
  if(gp != NULL)
  {
    ConfBridgeUser *usr = new ConfBridgeUser(ui->_tab2, _ami, arg1);
    QGridLayout* lyt = (QGridLayout*)gp->layout();
    lyt->addWidget(usr,lyt->rowCount(),0,1,1,(Qt::AlignLeft | Qt::AlignTop));
  }
}

void QtAsteriskDesktopMain::deleteConfBridgeUser(QVariantMap arg1)
{
  /*
    arg2:  "CallerIDname" :  QVariant(QString, "Daniel W Home PC")
    arg2:  "CallerIDnum" :  QVariant(uint, 4005)
    arg2:  "Channel" :  QVariant(QString, "SIP/4005-00000018")
    arg2:  "Conference" :  QVariant(uint, 1080)
    arg2:  "Event" :  QVariant(QString, "ConfbridgeLeave")
    arg2:  "Privilege" :  QVariant(QString, "call,all")
    arg2:  "Uniqueid" :  QVariant(QString, "1434153872.58")
*/
  QString conference = QString::number(arg1.value("Conference").toUInt());
  QGroupBox* gp =  findChild<QGroupBox*>(tr("_confBridge%1").arg(conference));
  if(gp != NULL)
  {
    ConfBridgeUser* usr = findChild<ConfBridgeUser*>(tr("cbu_%1_%2")
                                     .arg(conference)
                                     .arg(arg1.value("Uniqueid").toString())
    );
    if(usr != NULL)
    {
      QGridLayout* lyt = (QGridLayout*)gp->layout();
      lyt->removeWidget(usr);
      usr->deleteLater();
    }
  }
}

void QtAsteriskDesktopMain::confBridgeTalking(QVariantMap arg1)
{
/*
    arg2:  "Channel" :  QVariant(QString, "SIP/4004-00000015")
    arg2:  "Conference" :  QVariant(uint, 1080)
    arg2:  "Event" :  QVariant(QString, "ConfbridgeTalking")
    arg2:  "Privilege" :  QVariant(QString, "call,all")
    arg2:  "TalkingStatus" :  QVariant(QString, "off")
    arg2:  "Uniqueid" :  QVariant(QString, "1434153120.51")
*/
  ConfBridgeUser* usr = findChild<ConfBridgeUser*>(
        tr("cbu_%1_%2")
          .arg(arg1.value("Conference").toString())
          .arg(arg1.value("Uniqueid").toString())
  );
  if(usr != NULL)
  {
    usr->setTalkingStatus(arg1.value("TalkingStatus").toString() == "on");
  }
}

void QtAsteriskDesktopMain::sPreferences()
{
  qDebug() << "sPreferences";
  QtAsteriskDesktopPrefs * prefs = new QtAsteriskDesktopPrefs(this);
  prefs->exec();
}

void QtAsteriskDesktopMain::sDial()
{
  // Get the device
  QSettings set;
  if(set.contains("DEVICES/default"))
  {
    QString dvc = set.value("DEVICES/default").toString().replace("[]","");
    QString dialStr = dvc.replace("[^0-9a-zA-Z]","");
    //_ami->actionOriginate(dvc, ui->_dialNum->text(), "default", 1);
    _ami->actionOriginate(dialStr, ui->_dialNum->text(), "default", 1, QString(), QString(),0,tr("Originate: %1").arg(ui->_dialNum->text()));
  }
}

void QtAsteriskDesktopMain::sSetExtStatus(uint ext, AsteriskManager::ExtStatuses statuses)
{
  QSettings set;
  if(set.contains("DEVICES/default"))
  {
    if(set.contains("DEVICES/default"))
    {
      QString dvc = set.value("DEVICES/default").toString();
      QStringList parts = dvc.split("/");
      bool *valid = new bool(false);
      uint dvcExt = parts.at(1).toUInt(valid);
      if(valid && ext == dvcExt)
      {
        /*
    Removed     = -2,
    Deactivated = -1,
    NotInUse    = 0,
    InUse       = 1 << 0,
    Busy        = 1 << 1,
    Unavailable = 1 << 2,
    Ringing     = 1 << 3,
    OnHold      = 1 << 4
*/
        QString status;
        if(statuses.testFlag(AsteriskManager::Removed))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("Removed");
        if(statuses.testFlag(AsteriskManager::Deactivated))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("Deactivated");
        if(statuses.testFlag(AsteriskManager::NotInUse))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("NotInUse");
        if(statuses.testFlag(AsteriskManager::InUse))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("InUse");
        if(statuses.testFlag(AsteriskManager::Busy))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("Busy");
        if(statuses.testFlag(AsteriskManager::Unavailable))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("Unavailable");
        if(statuses.testFlag(AsteriskManager::Ringing))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("Ringing");
        if(statuses.testFlag(AsteriskManager::OnHold))
          status = tr("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
              .arg("OnHold");
        ui->_extStatus->setText(status);
      }
    }
  }
}

void QtAsteriskDesktopMain::sCallXfer(AstCall * call, const QString & exten)
{
  QMap<QString,AstChannel*> *chans = call->getChannels();
  if(chans->count() == 2)
  {
    QSettings s;
    if(s.contains("DEVICES/default"))
    {
      QString fullDvcStr = s.value("DEVICES/default").toString();
      QStringList dvcParts = fullDvcStr.split("/");
      if(dvcParts.count() == 2)
      {
        QString dvcType = dvcParts.at(0);
        QString dvcExten = dvcParts.at(1);
        QMap<QString, AstChannel *>::iterator i;
        for(i = chans->begin(); i != chans->end(); ++i)
        {
          ChanPart *cpart = i.value()->getChannelParts();
          if(cpart->getType() == dvcType && cpart->getExten() == dvcExten)
            continue;
          qDebug() << tr("I'm about to transfer channel %1 to %2").arg(i.value()->getChannel()).arg("730");
          _ami->actionRedirect(i.value()->getChannel(),"730","default",1,QString(),QString(),QString(),0);
        }
      }
    }
  }
}

void QtAsteriskDesktopMain::sCallPark(AstCall * call)
{
  QMap<QString,AstChannel*> *chans = call->getChannels();
  if(chans->count() == 2)
  {
    QSettings s;
    if(s.contains("DEVICES/default"))
    {
      QString fullDvcStr = s.value("DEVICES/default").toString();
      QStringList dvcParts = fullDvcStr.split("/");
      if(dvcParts.count() == 2)
      {
        QString dvcType = dvcParts.at(0);
        QString dvcExten = dvcParts.at(1);
        AstChannel *myChan = NULL;
        AstChannel *otherChan = NULL;
        QMap<QString, AstChannel *>::iterator i;
        for(i = chans->begin(); i != chans->end(); ++i)
        {
          ChanPart *cpart = i.value()->getChannelParts();
          if(cpart->getType() == dvcType && cpart->getExten() == dvcExten)
          {
            myChan = i.value();
          } else {
            otherChan = i.value();
          }
          if(NULL != myChan && NULL != otherChan)
          {
            break;
          }
        }
        if(NULL != myChan && NULL != otherChan)
        {
          qDebug() << QString("I'm about to park channel '%1' from channel '%2'")
                      .arg(otherChan->getChannel())
                      .arg(myChan->getChannel())
          ;
          int timeout = 45000;
          _ami->actionPark(otherChan->getChannel(),myChan->getChannel(),timeout);
        }
      }
    }
  }
}


void QtAsteriskDesktopMain::sCallHangup(AstCall * call)
{
  QMap<QString,AstChannel*> *chans = call->getChannels();
  if(chans->count() > 0)
  {
    QSettings s;
    if(s.contains("DEVICES/default"))
    {
      QString fullDvcStr = s.value("DEVICES/default").toString();
      QStringList dvcParts = fullDvcStr.split("/");
      if(dvcParts.count() == 2)
      {
        QString dvcType = dvcParts.at(0);
        QString dvcExten = dvcParts.at(1);
        QMap<QString, AstChannel *>::iterator i;
        for(i = chans->begin(); i != chans->end(); ++i)
        {
          ChanPart *cpart = i.value()->getChannelParts();
          if(cpart->getType() == dvcType && cpart->getExten() == dvcExten)
          {
            _ami->actionHangup(i.value()->getChannel(),16);
            return;
          } else {
            continue;
          }
        }
      }
    }
  }
}

void QtAsteriskDesktopMain::sDestroyingParkedCalled(AstParkedCall *parkedCall)
{
  if(_parkedMap->contains(parkedCall->getUuid()))
    _parkedMap->remove(parkedCall->getUuid());
}
void QtAsteriskDesktopMain::sPickUpParkedCall(AstParkedCall *parkedCall)
{
  // Get the device
  QSettings set;
  if(set.contains("DEVICES/default"))
  {
    QString dvc = set.value("DEVICES/default").toString();
    QString fromChan = dvc.replace("[^0-9a-zA-Z]","");
    //_ami->actionOriginate(dvc, ui->_dialNum->text(), "default", 1);
    _ami->actionOriginate(fromChan,
                          QString::number(parkedCall->getParkedExten(new bool)),
                          "from-internal",
                          1,
                          QString(),
                          QString(),
                          0,
                          QString("PRK %1: %2")
                            .arg(QString::number(parkedCall->getParkedExten(new bool)))
                            .arg(parkedCall->getChannelParked()->getCallIdStr())
    );
  }
}


void QtAsteriskDesktopMain::sDestroyingChannel(AstChannel *channel)
{
  if(_chanMap->contains(channel->getUuid()))
    _chanMap->remove(channel->getUuid());
}

void QtAsteriskDesktopMain::sDestroyingCall(AstCall *call)
{
  if(_callMap->contains(call->getUuid()))
    _callMap->remove(call->getUuid());
}
