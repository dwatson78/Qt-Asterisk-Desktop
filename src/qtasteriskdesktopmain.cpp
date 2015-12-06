#include "qtasteriskdesktopmain.h"
#include "ui_qtasteriskdesktopmain.h"

#include "qtasteriskdesktopprefs.h"

#include "asteriskmanager.h"
#include "confbridgeuser.h"
#include "admcallwidget.h"

#include "admvoicemailwidget.h"
#include "restapiastvm.h"
#include "admnotificationmanager.h"

#include "admstatic.h"

#include <QDebug>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <QStringList>
#include <QHostAddress>

QtAsteriskDesktopMain *_instance;

QtAsteriskDesktopMain::QtAsteriskDesktopMain(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::QtAsteriskDesktopMain)
{
  _instance = this;
  _phoneFeatures = NULL;

  ui->setupUi(this);

  _chanMap = new QMap<QString, AstChannel*>();
  _parkedMap = new QMap<QString, AstParkedCall*>();
  _callMap = new QMap<QString, AdmCallWidget*>();

  _extensionMap = new QMap<QString, AdmExtensionWidget *>();

  _mySipPeerMap = new QMap<QString, AstSipPeer*>();
  _sipPeerMap = new QMap<QString, AstSipPeer*>();

  _showSipPeerActionId = new QMap<QString, AstSipPeer*>();
  _extensionStateActionId = new QMap<QString, AstSipPeer*>();
  _loginActionId = QString();

  _sipPeersActionId = QString();
  _statusIcon = new StatusIcon(this);
  ui->statusBar->addPermanentWidget(_statusIcon);

  /*Connect the Menu Actions*/
  connect(ui->_actionExit,  SIGNAL(triggered()),
          this,             SLOT(sExit()));
  connect(ui->_actionPreferences, SIGNAL(triggered()),
          this,                  SLOT(sPreferences()));
  connect(ui->_actionPhoneFeatures, SIGNAL(triggered()),
          this,                     SLOT(sPhoneFeatures()));


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
    bool valid = false;
    
    QString host = settings.value(AmiPref::getName(AmiPref::host)).toString();
    uint port = settings.value(AmiPref::getName(AmiPref::port)).toUInt(&valid);
    if(valid)
    {
      _ami->connectToHost(host, port);
      bool connected = _ami->waitForConnected(60000);
      qDebug() << "connected: " << connected;
      qDebug() << _ami->socketType();
      qDebug() << _ami->peerName();

      qDebug() << _ami->peerAddress().toIPv4Address();
      qDebug() << _ami->peerPort();
      if(!connected)
      {
        qDebug() << _ami->errorString();
        _ami->connectToHost(host, port);
        bool connected = _ami->waitForConnected(60000);
        qDebug() << "connected: " << connected;
        qDebug() << _ami->socketType();
        qDebug() << _ami->peerName();

        qDebug() << _ami->peerAddress().toIPv4Address();
        qDebug() << _ami->peerPort();
        if(!connected)
        {
          qDebug() << _ami->errorString();
        }
      }
    }
    else
      asteriskDisconnected();
  }
  else
    asteriskDisconnected();

  settings.endGroup();

  if(settings.contains("DEVICES/default"))
  {
    AstChanParts cp(settings.value("DEVICES/default").toString());
    if(cp.isValid())
    {
      ui->_extStatus->setExten(cp.getExten());
    }
  }
}

QtAsteriskDesktopMain::~QtAsteriskDesktopMain()
{
  if(NULL != _phoneFeatures)
  {
    delete _phoneFeatures;
    _phoneFeatures = NULL;
  }
  delete ui;
  delete _statusIcon;
  delete _parkedMap;
  delete _callMap;
  delete _extensionMap;

  QList<AstSipPeer*> peers = _sipPeerMap->values();
  for(int i = 0; i < peers.count(); ++i)
  {
    peers.at(i)->blockSignals(true);
    delete peers.at(i);
  }
  delete _sipPeerMap;


  peers = _mySipPeerMap->values();
  for(int i = 0; i < peers.count(); ++i)
  {
    peers.at(i)->blockSignals(true);
    delete peers.at(i);
  }
  delete _mySipPeerMap;

  delete _chanMap;
  delete _showSipPeerActionId;
  delete _extensionStateActionId;
  delete _ami;
  _instance = NULL;
}

void QtAsteriskDesktopMain::asteriskConnected()
{
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
  Q_UNUSED(arg1)

  _statusIcon->setConnected(true);
}

void QtAsteriskDesktopMain::asteriskDisconnected()
{
  qWarning() << "QtAsteriskDesktopMain::asteriskDisconnected: Asterisk Disconnected!";
}

void QtAsteriskDesktopMain::asteriskResponseSent(AsteriskManager::Response arg1, QVariantMap arg2, QString arg3)
{
  switch(arg1)
  {
    case (AsteriskManager::Success):
    {
#ifdef AST_DEBUG
      QString catchMe = AdmStatic::eventToString(arg2,"(2004|1998)");
      if(!catchMe.isNull() && !catchMe.isEmpty())
      {
        qDebug() << "QtAsteriskDesktopMain::asteriskResponseSent:"
                 << catchMe
        ;
      }
#endif
      if(_loginActionId.isNull() == false && arg3 == _loginActionId)
      {
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
        _loginActionId = QString();
        _sipPeersActionId = _ami->actionSIPpeers();
      } else if(_sipPeersActionId.isNull() == false && arg3 == _sipPeersActionId) {
        _sipPeersActionId = QString();

        // Setup the extensions programs
        AdmExtensionWidget *peerWidget;
        peerWidget = new AdmExtensionWidget();
        peerWidget->setExten(QString("730"));
        peerWidget->setText("Tracking #");
        peerWidget->setPixmap(QPixmap(":/icons/status-ring.png"));
        ui->_layoutPrograms->addWidget(peerWidget);
        connect(peerWidget,           SIGNAL(sigDragEnterEvent(AdmExtensionWidget*,QDragEnterEvent*)),
                ui->_scrollPrograms,  SLOT(sDragEnterEvent(AdmExtensionWidget*,QDragEnterEvent*)));
      } else if(_showSipPeerActionId->count() > 0 && _showSipPeerActionId->contains(arg3)) {
        AstSipPeer *peer = _showSipPeerActionId->value(arg3);
        _showSipPeerActionId->remove(arg3);
        if(arg1 == AsteriskManager::Success)
        {
          peer->sResponseShowSipPeer(arg2);
          if(_mySipPeerMap->contains(peer->getObjectName().toString()))
          {
            if(!peer->getVmBox().isNull())
            {
              AdmVoiceMailWidget *w = new AdmVoiceMailWidget(peer->getVmBox().toString());
              connect(w,    SIGNAL(sigPlayMsgOnPhone(AdmVoiceMailWidget*,QVariantMap)),
                      this, SLOT(sPlayMsgOnPhone(AdmVoiceMailWidget*,QVariantMap)));
              ui->_voiceMailTabWidget->addVoiceMailWidget(w);
            }
          }
        }
      } else if (_extensionStateActionId->count() > 0 && _extensionStateActionId->contains(arg3)) {
        AstSipPeer *peer = _extensionStateActionId->value(arg3);
        _extensionStateActionId->remove(arg3);
        if(arg1 == AsteriskManager::Success && peer)
        {
          QString catchMe = AdmStatic::eventToString(arg2);
          if(!catchMe.isNull() && !catchMe.isEmpty())
          {
            qDebug() << "QtAsteriskDesktopMain::asteriskResponseSent:"
                     << catchMe
            ;
          }
          peer->sExtensionStatusEvent(arg2);
          if(peer->getObjectName().toString() == "2004"
             || peer->getObjectName().toString() == "1998")
          {
            qDebug() << peer->getObjectName().toString() << " changed once again!";
          }
        }
      }
      break;
    }
    case (AsteriskManager::Error):
    {
      qCritical() << QString("QtAsteriskDesktopMain::asteriskResponseSent: ERROR: actionId: %1").arg(arg3);
      for(QVariantMap::const_iterator iter = arg2.begin();
          iter != arg2.end();
          ++iter)
      {
        qWarning() << "QtAsteriskDesktopMain::asteriskResponseSent: " << iter.key() << ": " << iter.value();
      }
      return;
    }
    case (AsteriskManager::Follows):
    {
      qCritical() << QString("QtAsteriskDesktopMain::asteriskResponseSent: Status UNHANDLED: [AsteriskManager::Follows]: actionId: %1");
      for(QVariantMap::const_iterator iter = arg2.begin();
          iter != arg2.end();
          ++iter)
      {
        qWarning() << "QtAsteriskDesktopMain::asteriskResponseSent: " << iter.key() << ": " << iter.value();
      }
      return;
    }
  }
}

void QtAsteriskDesktopMain::asteriskEventGenerated(AsteriskManager::Event arg1, QVariantMap arg2)
{
#ifdef AST_DEBUG
  QString catchMe = AdmStatic::eventToString(arg2,"(2004|1998)");
  if(!catchMe.isNull() && !catchMe.isEmpty())
  {
    qDebug() << "QtAsteriskDesktopMain::asteriskEventGenerated:"
             << catchMe
    ;
  }
#endif
  //Skip the following events
  switch(arg1)
  {
    case AsteriskManager::VarSet:
    case AsteriskManager::Newexten:
      return;
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
      if(arg2.contains("Uniqueid") && !arg2.value("Uniqueid").toString().trimmed().isEmpty())
      {
        AstChannel *chan = new AstChannel(arg2.value("Uniqueid").toString(), arg2, this);
        connect(chan, SIGNAL(destroying(AstChannel*)),
                this, SLOT(sDestroyingChannel(AstChannel*))
        );
        _chanMap->insert(chan->getUuid(), chan);
      }
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
      else if(arg2.contains("ParkeeUniqueid"))
        uuid = arg2.value("ParkeeUniqueid",QString()).toString();
      else
        uuid = QString();
      QString fromChannelName=QString();
      QString fromChannelUuid=QString();
      if(arg2.contains("From"))
        fromChannelName = arg2.value("From").toString();
      else if(arg2.contains("ParkeeLinkedid"))
        fromChannelUuid = arg2.value("ParkeeLinkedid").toString();

      if(uuid != QString() && (fromChannelName != QString() || fromChannelUuid != QString()) && _chanMap->contains(uuid))
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
            if (!fromChannelUuid.isNull() && i.value()->getUuid() == fromChannelUuid)
            {
              chanFrom = i.value();
              break;
            }
            else if(!fromChannelName.isEmpty() &&  i.value()->getChannel() == fromChannelName)
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
        else if(arg2.contains("ParkeeUniqueid"))
          uuid = arg2.value("ParkeeUniqueid",QString()).toString();
        else
          uuid = QString();
        if(uuid != QString() && _parkedMap->contains(uuid))
        {
          _parkedMap->value(uuid)->sParkedCallEvent(arg1, arg2);
        }
      }
      // Do not add a break here
    }
    case AsteriskManager::Hangup:
    case AsteriskManager::Newstate:
    case AsteriskManager::NewCallerid:
    case AsteriskManager::Rename:
    case AsteriskManager::MusicOnHold:
    {
      //Get the Uniqueid
      QString uuid1,uuid2;

      if(arg2.contains("Uniqueid"))
        uuid1 = arg2.value("Uniqueid",QString()).toString();
      else if(arg2.contains("UniqueId"))
        uuid1 = arg2.value("UniqueId",QString()).toString();
      else if(arg2.contains("UniqueID"))
        uuid1 = arg2.value("UniqueID",QString()).toString();
      else if(arg2.contains("ParkeeUniqueid"))
        uuid1 = arg2.value("ParkeeUniqueid",QString()).toString();
      else
        uuid1 = QString();

      if(arg2.contains("RetrieverUniqueid"))
        uuid2 = arg2.value("RetrieverUniqueid",QString()).toString();
      else
        uuid2 = QString();

      if(uuid1 != QString() && _chanMap->contains(uuid1))
      {
        AstChannel *chan = _chanMap->value(uuid1,0);
        chan->sChannelEvent(arg1, arg2);
      } else {
        qWarning() << QString("QtAsteriskDesktopMain::asteriskEventGenerated: Could not find channel for: uuid: %1, event: %2")
                      .arg(uuid1)
                      .arg(arg2.contains("Event") ? arg2.value("Event").toString() : QString::number(arg1));
      }
      if(uuid2 != QString() && _chanMap->contains(uuid2))
      {
        AstChannel *chan = _chanMap->value(uuid2,0);
        chan->sChannelEvent(arg1, arg2);
      }
      break;
    }
    case AsteriskManager::Bridge:
    case AsteriskManager::BridgeEnter:
    {
      if(arg2.contains("Bridgestate") && arg2.value("Bridgestate").toString() != "Link")
        break;
      else
      {
        QString uuid1,uuid2;
        if(arg2.contains("Uniqueid1"))
          uuid1 = arg2.value("Uniqueid1").toString();
        else if(arg2.contains("Linkedid"))
          uuid1 = arg2.value("Linkedid").toString();
        if(arg2.contains("Uniqueid2"))
          uuid2 = arg2.value("Uniqueid2").toString();
        else if(arg2.contains("Uniqueid"))
          uuid2 = arg2.value("Uniqueid").toString();

        if(uuid1.isNull())
        {
          qWarning() << "QtAsteriskDesktopMain::asteriskEventGenerated: uuid1 could not be found.";
          break;
        }
        else if(uuid2.isNull())
        {
          qWarning() << "QtAsteriskDesktopMain::asteriskEventGenerated: uuid2 could not be found.";
          break;
        }
        else if (uuid1 == uuid2)
        {
          qWarning() << "QtAsteriskDesktopMain::asteriskEventGenerated: uuid1 and uuid2 are the same value.";
          //break;
        }

        // Call handling
        if(!_callMap->contains(uuid1))
        {
          AdmCallWidget *call = new AdmCallWidget();
          connect(call, SIGNAL(callXfer(AdmCallWidget*,QString)),
                  this, SLOT(sCallXfer(AdmCallWidget*,QString))
          );
          connect(call, SIGNAL(callPark(AdmCallWidget*)),
                  this, SLOT(sCallPark(AdmCallWidget*))
          );
          connect(call, SIGNAL(callHangup(AdmCallWidget*)),
                  this, SLOT(sCallHangup(AdmCallWidget*))
          );
          connect(call, SIGNAL(destroying(AdmCallWidget*)),
                  this, SLOT(sDestroyingCall(AdmCallWidget*))
          );

          if(_chanMap->contains(uuid1))
            call->addChannel(_chanMap->value(uuid1));

          if(_chanMap->contains(uuid2))
            call->addChannel(_chanMap->value(uuid2));

          _callMap->insert(uuid1,call);

          ui->_layoutCalls->addWidget(call);
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
          //Disable the music on hold status
          _chanMap->value(uuid)->sMusicOff(arg2);
          AdmCallWidget *call = new AdmCallWidget();
          connect(call, SIGNAL(callXfer(AdmCallWidget*,QString)),
                  this, SLOT(sCallXfer(AdmCallWidget*,QString))
          );
          connect(call, SIGNAL(callPark(AdmCallWidget*)),
                  this, SLOT(sCallPark(AdmCallWidget*))
          );
          connect(call, SIGNAL(callHangup(AdmCallWidget*)),
                  this, SLOT(sCallHangup(AdmCallWidget*))
          );
          connect(call, SIGNAL(destroying(AdmCallWidget*)),
                  this, SLOT(sDestroyingCall(AdmCallWidget*))
          );
          call->addChannel(_chanMap->value(uuid));

          QString destUuid = arg2.value("DestUniqueID").toString();
          if(_chanMap->contains(destUuid))
            call->addChannel(_chanMap->value(destUuid));

          _callMap->insert(uuid,call);

          ui->_layoutCalls->addWidget(call);
        }
      }
      break;
    }
    case AsteriskManager::PeerEntry:
    {
      AstSipPeer *peer = new AstSipPeer(arg2);
      QMap<QString, AstSipPeer *> *sipPeerMap = NULL;
      if(peer->getMyDevice())
        sipPeerMap = _mySipPeerMap;
      else
        sipPeerMap = _sipPeerMap;

      sipPeerMap->insert(peer->getObjectName().toString(),peer);
      QString dndActionId = _ami->actionDBGet("CustomDevstate",QString("DEVDND%1").arg(peer->getObjectName().toString()));
      qDebug() << QString("QtAsteriskDesktopMain::asteriskEventGenerated: actionDBGet: peer: %1, actionId: %2").arg(peer->getObjectName().toString()).arg(dndActionId);

      connect(peer, SIGNAL(destroying(AstSipPeer*)),
              this, SLOT(sDestroyingSipPeer(AstSipPeer*)));

      if(sipPeerMap == _sipPeerMap)
      {
        AdmExtensionWidget *peerWidget = new AdmExtensionWidget();
        _extensionMap->insert(peer->getObjectName().toString(), peerWidget);
        connect(peerWidget,             SIGNAL(sigDragEnterEvent(AdmExtensionWidget*,QDragEnterEvent*)),
                ui->_scrollPeersAvail,  SLOT(sDragEnterEvent(AdmExtensionWidget*,QDragEnterEvent*)));
        connect(peerWidget, SIGNAL(destroying(AdmExtensionWidget*)),
                this,       SLOT(sDestroyingAdmExtensionWidget(AdmExtensionWidget*)));
        peerWidget->setExten(peer->getObjectName());
        peerWidget->setText(peer->getDescription());
        peerWidget->setPixmap(QPixmap(":/icons/status-avail.png"));
        peerWidget->setSipPeer(peer);
        ui->_layoutSipPeersAvail->addWidget(peerWidget);
      } else if (sipPeerMap == _mySipPeerMap) {
        ui->_extStatus->setSipPeer(peer);
      }

      QString actionId = _ami->actionSIPshowpeer(peer->getObjectName().toString());
      _showSipPeerActionId->insert(actionId,peer);

      actionId = _ami->actionExtensionState(peer->getObjectName().toString());
      _extensionStateActionId->insert(actionId,peer);
      break;
    }
    case AsteriskManager::PeerStatus:
    {
      if(arg2.contains("Peer"))
      {
        QString peerName = arg2.value("Peer").toString();
        if(arg2.contains("ChannelType"))
        {
          QString chanType = arg2.value("ChannelType").toString();
          if(chanType == "SIP")
          {
            if(peerName.startsWith("SIP/"))
              peerName = peerName.replace(0,4,"");

            AstSipPeer *peer = NULL;
            if(_sipPeerMap->contains(peerName))
              peer = _sipPeerMap->value(peerName);
            if(_mySipPeerMap->contains(peerName))
            {
              peer = _mySipPeerMap->value(peerName);
            }

            if(NULL != peer)
              peer->sPeerStatusEvent(arg2);
          }
        }
      }
      break;
    }
    case AsteriskManager::ExtensionStatus:
    {
      QVariant exten = QVariant();
      if(arg2.contains("Exten"))
        exten = arg2.value("Exten");

      AstSipPeer *peer = NULL;
      QString sipPeerName = QString();
      if(arg2.contains("Hint"))
      {
        QString hint = arg2.value("Hint").toString();
        QRegExp re("^.*SIP/([^&]+).*$");
        if(re.exactMatch(hint) && re.captureCount() == 1)
        {
          sipPeerName = re.cap(1);
          if(_sipPeerMap->contains(sipPeerName))
            peer = _sipPeerMap->value(sipPeerName);
          else if (_mySipPeerMap->contains(sipPeerName))
            peer = _mySipPeerMap->value(sipPeerName);
        }
      }
      if(_extensionMap->contains(exten.toString()))
      {
        AdmExtensionWidget *widget = _extensionMap->value(exten.toString());
        if(NULL != peer && peer != widget->getSipPeer())
        {
          widget->setSipPeer(peer);
        }
        widget->sExtensionStatusEvent(arg2);
        if(peer->getObjectName().toString() == "2004"
           || peer->getObjectName().toString() == "1998")
        {
          qDebug() << peer->getObjectName().toString() << " changed once again!";
        }
      } else if(exten.toString().startsWith("*76")) {
        QRegExp re("^\\*76(.*)$");
        if(re.exactMatch(exten.toString()))
        {
          exten.setValue(re.cap(1));
          if(_extensionMap->contains(exten.toString()))
            _extensionMap->value(exten.toString())->sExtensionDndStatusEvent(arg2);
          if(_sipPeerMap->contains(exten.toString()))
            _sipPeerMap->value(exten.toString())->sDndStatusEvent(arg2);
          else if(_mySipPeerMap->contains(exten.toString()))
            _mySipPeerMap->value(exten.toString())->sDndStatusEvent(arg2);
        }
      } else if(NULL != peer /*&& exten.toString() != sipPeerName*/) {
        peer->sExtensionStatusEvent(arg2);
        if(peer->getObjectName().toString() == "2004"
           || peer->getObjectName().toString() == "1998")
        {
          qDebug() << peer->getObjectName().toString() << " changed once again!";
        }
      }
      break;
    }
    default:
      if(arg2.contains("Event"))
      {
        if(arg2.value("Event").toString() == "DBGetResponse")
        {
          /*
            catchMeIfYouCan: Begin Caught Event Info
            catchMeIfYouCan: arg1: Event:
            catchMeIfYouCan: arg2:  "ActionID" :  QVariant(QString, "{de20209a-3399-4755-960a-19ca404fee12}")
            catchMeIfYouCan: arg2:  "Event" :  QVariant(QString, "DBGetResponse")
            catchMeIfYouCan: arg2:  "Family" :  QVariant(QString, "CustomDevstate")
            catchMeIfYouCan: arg2:  "Key" :  QVariant(QString, "DEVDND4005")
            catchMeIfYouCan: arg2:  "Val" :  QVariant(QString, "BUSY")
            catchMeIfYouCan: End Caught Event Info
          */
          QString fam, key;
          if(arg2.contains("Family"))
            fam = arg2.value("Family").toString();
          if(fam == "CustomDevstate")
          {
            if(arg2.contains("Key"))
              key = arg2.value("Key").toString();
            QRegExp re("^DEVDND(.*)$");
            if(re.exactMatch(key))
            {
              AstSipPeer *peer = NULL;
              QString sipPeerName = re.cap(1);
              if(_sipPeerMap->contains(sipPeerName))
                peer = _sipPeerMap->value(sipPeerName);
              else if (_mySipPeerMap->contains(sipPeerName))
                peer = _mySipPeerMap->value(sipPeerName);
              if(NULL != peer)
              {
                if(arg2.contains("Val"))
                {
                  bool isDndOn = arg2.value("Val").toString() == "BUSY";
                  peer->setDnd(isDndOn);
                }
              }
            }
          }
        }
      }
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

void QtAsteriskDesktopMain::sExit()
{
  QApplication::quit();
}
void QtAsteriskDesktopMain::sPreferences()
{
  QtAsteriskDesktopPrefs * prefs = new QtAsteriskDesktopPrefs(this);
  prefs->exec();
}
void QtAsteriskDesktopMain::sPhoneFeatures()
{
  if(NULL ==_phoneFeatures)
  {
    _phoneFeatures = new DlgPhoneFeatures(this);
  }
  if(!_phoneFeatures->isVisible())
  {
    _phoneFeatures->show();
  } else {
    _phoneFeatures->activateWindow();
  }
}

void QtAsteriskDesktopMain::sDial()
{
  // Get the device
  QSettings set;
  if(set.contains("DEVICES/default"))
  {
    AstChanParts cp(set.value("DEVICES/default").toString());
    if(cp.isValid())
    {
      QString dvc = cp.getType().append("/").append(cp.getExten());
      _ami->actionOriginate(dvc, ui->_dialNum->text(), "default", 1, QString(), QString(),0,tr("Originate: %1").arg(ui->_dialNum->text()));
    }
  }
}

void QtAsteriskDesktopMain::sSetExtStatus(uint ext, AsteriskManager::ExtStatuses statuses)
{
  QSettings set;
  if(set.contains("DEVICES/default"))
  {
    AstChanParts cp(set.value("DEVICES/default").toString());
    if(cp.isValid())
    {
      if(cp.getExten().toUInt() == ext)
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

void QtAsteriskDesktopMain::sCallXfer(AdmCallWidget * call, const QString & exten)
{
  QMap<QString,AstChannel*> *chans = call->getChannels();
  if(chans->count() == 2)
  {
    QSettings s;
    if(s.contains("DEVICES/default"))
    {
      AstChanParts cp(s.value("DEVICES/default").toString());
      if(cp.isValid())
      {
        QMap<QString, AstChannel *>::iterator i;
        for(i = chans->begin(); i != chans->end(); ++i)
        {
          AstChanParts *cpart = i.value()->getChannelParts();
          if(cpart->getType() == cp.getType() && cpart->getExten() == cp.getExten())
            continue;
          _ami->actionRedirect(i.value()->getChannel(),exten,"default",1,QString(),QString(),QString(),0);
        }
      }
    }
  }
}

void QtAsteriskDesktopMain::sCallPark(AdmCallWidget * call)
{
  QMap<QString,AstChannel*> *chans = call->getChannels();
  if(chans->count() == 2)
  {
    QSettings s;
    if(s.contains("DEVICES/default"))
    {
      AstChanParts cp(s.value("DEVICES/default").toString());
      if(cp.isValid())
      {
        AstChannel *myChan = NULL;
        AstChannel *otherChan = NULL;
        QMap<QString, AstChannel *>::iterator i;
        for(i = chans->begin(); i != chans->end(); ++i)
        {
          AstChanParts *cpart = i.value()->getChannelParts();
          if(cpart->getType() == cp.getType() && cpart->getExten() == cp.getExten())
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
          int timeout = 45000; // 45 seconds
          _ami->actionPark(otherChan->getChannel(),myChan->getChannel(),timeout);
        }
      }
    }
  }
}


void QtAsteriskDesktopMain::sCallHangup(AdmCallWidget * call)
{
  QMap<QString,AstChannel*> *chans = call->getChannels();
  if(chans->count() > 0)
  {
    QSettings s;
    if(s.contains("DEVICES/default"))
    {
      AstChanParts cp(s.value("DEVICES/default").toString());
      if(cp.isValid())
      {
        QMap<QString, AstChannel *>::iterator i;
        for(i = chans->begin(); i != chans->end(); ++i)
        {
          AstChanParts *cpart = i.value()->getChannelParts();
          if(cpart->getType() == cp.getType() && cpart->getExten() == cp.getExten())
          {
            //Determine the state of the channel and then use the correct code
            bool ok = false;
            uint chanStateVal = i.value()->getChannelState(&ok);
            if(ok)
            {
              AsteriskManager::ChanState chanState =(AsteriskManager::ChanState)chanStateVal;
              uint hangupCause = ((chanState == AsteriskManager::ChanStateUp) ? 16 : 21);
              _ami->actionHangup(i.value()->getChannel(),hangupCause);
            }
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
    AstChanParts cp(set.value("DEVICES/default").toString());
    if(cp.isValid())
    {
      AstChanParts *parts = parkedCall->getChannelParked()->getChannelParts();
      if(parts->getType() != cp.getType() || parts->getExten() != cp.getExten())
      {
        _ami->actionRedirect(parkedCall->getChannelParked()->getChannel(),
                             cp.getExten(), // The default device extension number
                             "from-internal", // TODO: See if there is a dialplan that could be created for this
                             1);
      }
    }
  }
}

void QtAsteriskDesktopMain::sDestroyingChannel(AstChannel *channel)
{
  if(_chanMap->contains(channel->getUuid()))
    _chanMap->remove(channel->getUuid());
}

void QtAsteriskDesktopMain::sDestroyingCall(AdmCallWidget *call)
{
  if(_callMap->contains(call->getUuid()))
    _callMap->remove(call->getUuid());
}
void QtAsteriskDesktopMain::sDestroyingSipPeer(AstSipPeer *peer)
{
  disconnect( peer, SIGNAL(destroying(AstSipPeer*)),
              this, SLOT(sDestroyingSipPeer(AstSipPeer*)));

  if(_sipPeerMap->contains(peer->getObjectName().toString()))
    _sipPeerMap->remove(peer->getObjectName().toString());
  else if(_mySipPeerMap->contains(peer->getObjectName().toString()))
  {
    _mySipPeerMap->remove(peer->getObjectName().toString());
  }
}

void QtAsteriskDesktopMain::sDestroyingAdmExtensionWidget(AdmExtensionWidget *widget)
{
  if(_extensionMap->contains(widget->getExten()))
    _extensionMap->remove(widget->getExten());
}

void QtAsteriskDesktopMain::sPlayMsgOnPhone(AdmVoiceMailWidget *obj, const QVariantMap &data)
{
  QSettings set;
  if(set.contains("DEVICES/default"))
  {
    AstChanParts cp(set.value("DEVICES/default").toString());
    if(cp.isValid())
    {
      if(data.contains("sndfile") && data.contains("vmFolder"))
      {
        QString sndFile = data.value("sndfile").toString();
        sndFile = sndFile.mid(0,sndFile.length()-4);
        //TODO: Get sndFile from Rest API using the msg_id value...
        QString vmFolderFile = data.value("vmFolder").toString();
        _ami->actionOriginate(
                cp.getType().append("/").append(cp.getExten()),
                QString(),
                QString(),
                0,
                "Playback",
                QString("/var/spool/asterisk/voicemail/default/%1/%2/%3")
                        .arg(obj->getVmBox())
                        .arg(vmFolderFile)
                        .arg(sndFile),
                10000, // 10 second timeout
                QString("VM: %1").arg(data.value("callerid").toString())
        );
      }
    }
  }
}
