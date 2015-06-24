#include "astsippeer.h"

#include <QSettings>
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

  _status = QString();
  if(event.contains("Status"))
    _status = event.value("Status").toString();

  _textSupport = false;
  if(event.contains("TextSupport"))
    _textSupport = event.value("TextSupport").toString().toLower() == "yes";

  _videoSupport = false;
  if(event.contains("VideoSupport"))
    _videoSupport = event.value("VideoSupport").toString().toLower() == "yes";

  _myDevice = false;
  QSettings s;
  if(s.contains("DEVICES/default"))
  {
    QString peerDvc = QString("%1/%2").arg(_channelType).arg(_objectName.toString());
    if(s.value("DEVICES/default").toString() == peerDvc)
      _myDevice = true;
  }
}

AstSipPeer::~AstSipPeer()
{
  emit destroying(this);
}

void AstSipPeer::sUpdatePeer(const QVariantMap &event)
{
  Q_UNUSED(event);
}
