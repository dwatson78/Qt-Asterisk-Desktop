#include "astsippeer.h"

#include <QDebug>
#include <QSettings>
#include <QRegExp>

#include "asteriskmanager.h"
#include "astchanparts.h"

AstSipPeer::AstSipPeer(const QVariantMap &event, QObject *parent) :
  QObject(parent)
{
  _acl = false;
  if(event.contains("ACL"))
    _acl = event.value("ACL").toString().toLower() == "yes";

  _autoComedia = false;
  if(event.contains("AutoComedia"))
    _autoComedia = event.value("AutoComedia").toString().toLower() == "yes";

  _autoForceRPort = false;
  if(event.contains("AutoForcerport"))
    _autoForceRPort = event.value("AutoForcerport").toString().toLower() == "yes";

  _chanObjectType = QString();
  if(event.contains("ChanObjectType"))
    _chanObjectType = event.value("ChanObjectType").toString();

  _channelType = QString();
  if(event.contains("Channeltype"))
    _channelType = event.value("Channeltype").toString();

  _comedia = false;
  if(event.contains("Comedia"))
    _comedia = event.value("Comedia").toString().toLower() == "yes";

  _description = QString();
  if(event.contains("Description"))
    _description = event.value("Description").toString();

  _dynamic = false;
  if(event.contains("Dynamic"))
    _dynamic = event.value("Dynamic").toString().toLower() == "yes";

  _forceRPort = false;
  if(event.contains("Forcerport"))
    _forceRPort = event.value("Forcerport").toString().toLower() == "yes";

  _ipAdress = QString();
  if(event.contains("IPaddress"))
    _ipAdress = event.value("IPaddress").toString();

  _ipPort = QVariant(QVariant::UInt);
  if(event.contains("IPport") && !event.value("IPport").toString().isEmpty())
    _ipPort = event.value("IPport");

  _objectName = QVariant(QVariant::UInt);
  if(event.contains("ObjectName") && !event.value("ObjectName").toString().isEmpty())
    _objectName = event.value("ObjectName");

  _realTimeDevice = false;
  if(event.contains("RealtimeDevice"))
    _realTimeDevice = event.value("RealtimeDevice").toString().toLower() == "yes";

  _peerStatus = QString();
  if(event.contains("Status"))
    _peerStatus = event.value("Status").toString();

  _extStatus = AsteriskManager::Unavailable;

  _textSupport = false;
  if(event.contains("TextSupport"))
    _textSupport = event.value("TextSupport").toString().toLower() == "yes";

  _videoSupport = false;
  if(event.contains("VideoSupport"))
    _videoSupport = event.value("VideoSupport").toString().toLower() == "yes";

  AstChanParts cp(QString("%1/%2").arg(_channelType).arg(_objectName.toString()));
  _myDevice = cp.isMyDevice();

  _isDndOn = false;
}

AstSipPeer::~AstSipPeer()
{
  emit destroying(this);
}

void AstSipPeer::sPeerStatusEvent(const QVariantMap &event)
{
  QString prevPeerStatus = QString(_peerStatus);
  if(event.contains("PeerStatus"))
  {
    QString peerStatus = event.value("PeerStatus").toString();
    if(peerStatus == "Registered" || peerStatus == "Reachable")
    {
      _peerStatus = peerStatus;
      if(event.contains("Address"))
      {
        QStringList addrParts = event.value("Address").toString().split(":");
        _ipAdress = addrParts.at(0);
        _ipPort = addrParts.count() == 2 ? addrParts.at(1).toUInt() : QVariant(QVariant::UInt);
      }
    } else if(peerStatus == "Unregistered" || peerStatus == "Unreachable") {
      _peerStatus = peerStatus;
      _ipAdress = QString();
      _ipPort = QVariant(QVariant::UInt);
    }
  }
  if(prevPeerStatus != _peerStatus)
    emit sUpdated(this);
}

void AstSipPeer::sResponseShowSipPeer(const QVariantMap &event)
{
  /*arg2:  "Address-IP" :  QVariant(QString, "10.121.212.154")
arg2:  "Address-Port" :  QVariant(uint, 5060)
arg2:  "Busy-level" :  QVariant(uint, 0)
arg2:  "Call-limit" :  QVariant(uint, 2147483647)
arg2:  "Callerid" :  QVariant(QString, ""Daniel W Desk" <2004>")
arg2:  "Callgroup" :  QVariant(QString, "")
arg2:  "Context" :  QVariant(QString, "from-internal")
arg2:  "Default-Username" :  QVariant(uint, 2004)
arg2:  "ObjectName" :  QVariant(uint, 2004)
arg2:  "Pickupgroup" :  QVariant(QString, "")
arg2:  "Reg-Contact" :  QVariant(QString, "sip:2004@10.121.212.154")
arg2:  "SIP-Useragent" :  QVariant(QString, "PolycomVVX-VVX_300-UA/5.3.0.12074")
arg2:  "Status" :  QVariant(QString, "OK (24 ms)")
arg2:  "VoiceMailbox" :  QVariant(uint, 5004)
arg3:  "{b5e85124-23b3-4c32-b024-335f0a6a0b8c}" */
  QString varName = "Address-IP";
  if(event.contains(varName))
    _ipAdress = event.value(varName).toString();

  varName = "Address-Port";
  if(event.contains(varName))
    _ipPort = event.value(varName);

  varName = "Busy-level";
  if(event.contains(varName))
   _busyLevel = event.value(varName);

  varName = "Call-limit";
  if(event.contains(varName))
    _callLimit = event.value(varName);

  varName = "Callerid";
  if(event.contains(varName))
    _callerId = event.value(varName).toString();
  if(!_callerId.isNull() && !_callerId.trimmed().isEmpty())
  {
    QRegExp re("\"((?:[^\\\"]|\\.)*)\"\\ <(.*)>$");
    if(re.exactMatch(_callerId))
    {
      _description = re.cap(1);
    }
  }

  varName = "Callgroup";
  if(event.contains(varName))
    _callGroup = event.value(varName).toString();

  varName = "Context";
  if(event.contains(varName))
    _context = event.value(varName).toString();

  varName = "Default-Username";
  if(event.contains(varName))
    _defaultUsername = event.value(varName).toString();

  /*varName = "ObjectName";
  if(event.contains(varName))
     = event.value(varName);*/

  varName = "Pickupgroup";
  if(event.contains(varName))
    _pickupGroup = event.value(varName).toString();

  varName = "Reg-Contact";
  if(event.contains(varName))
    _regContact = event.value(varName).toString();

  varName = "SIP-Useragent";
  if(event.contains(varName))
    _userAgent = event.value(varName).toString();

  varName = "Status";
  if(event.contains(varName))
    _peerStatus = event.value(varName).toString();

  varName = "VoiceMailbox";
  if(event.contains(varName))
    _vmBox = event.value(varName);

  emit sUpdated(this);
}

void AstSipPeer::sExtensionStatusEvent(const QVariantMap &event)
{
  if(event.contains("Status"))
  {
    bool ok = false;
    uint status = event.value("Status").toUInt(&ok);
    if(ok)
      _extStatus = (AsteriskManager::ExtStatus)status;
  }
  emit sigExtensionStatusEvent(this, event);
}

void AstSipPeer::sDndStatusEvent(const QVariantMap &event)
{
  if(event.contains("Status"))
  {
    uint statusNum = event.value("Status").toUInt();
    AsteriskManager::ExtStatuses statuses(statusNum);
    _isDndOn = !statuses.testFlag(AsteriskManager::NotInUse);
    emit sUpdated(this);
    emit sigDndStatusEvent(this, event, _isDndOn);
  }
}

void AstSipPeer::setDnd(bool isDndOn)
{
  _isDndOn=isDndOn;
  emit sUpdated(this);
  emit sigDndStatusEvent(this, QVariantMap(), _isDndOn);
}
