#ifndef ASTSIPPEER_H
#define ASTSIPPEER_H

#include <QObject>
#include <QVariantMap>

class AstSipPeer : public QObject
{
  Q_OBJECT
public:
  explicit AstSipPeer(const QVariantMap &event, QObject *parent = 0);
  ~AstSipPeer();

  bool            getAcl()            {return _acl;}
  bool            getAutoComedia()    {return _autoComedia;}
  bool            getAutoForceRPort() {return _autoForceRPort;}
  const QString&  getChanObjectType() {return _chanObjectType;}
  const QString&  getChannelType()    {return _channelType;}
  bool            getComedia()        {return _comedia;}
  const QString&  getDescription()    {return _description;}
  bool            getDynamic()        {return _dynamic;}
  bool            getForceRPort()     {return _forceRPort;}
  const QString&  getIpAdress()       {return _ipAdress;}
  const QVariant& getIpPort()         {return _ipPort;}
  const QVariant& getObjectName()     {return _objectName;}
  bool            getRealTimeDevice() {return _realTimeDevice;}
  const QString&  getStatus()         {return _status;}
  bool            getTextSupport()    {return _textSupport;}
  bool            getVideoSupport()   {return _videoSupport;}
  bool            getMyDevice()       {return _myDevice;}
  const QVariant& getVmBox()          {return _vmBox;}

  void setDnd(bool isDndOn);

public slots:
  void sPeerStatusEvent(const QVariantMap &event);
  void sResponseShowSipPeer(const QVariantMap &event);
  void sExtensionStatusEvent(const QVariantMap &event);
  void sDndStatusEvent(const QVariantMap &event);

protected:

signals:
  void destroying(AstSipPeer *);
  void sUpdated(AstSipPeer *);
  void sigExtensionStatusEvent(AstSipPeer *, const QVariantMap &event);
  void sigDndStatusEvent(AstSipPeer *, const QVariantMap &event, bool);

private:
  bool      _acl;
  bool      _autoComedia;
  bool      _autoForceRPort;
  QString   _chanObjectType;
  QString   _channelType;
  bool      _comedia;
  QString   _description;
  bool      _dynamic;
  bool      _forceRPort;
  QString   _ipAdress;
  QVariant  _ipPort;
  QVariant  _objectName;
  bool      _realTimeDevice;
  QString   _status;
  bool      _textSupport;
  bool      _videoSupport;

  bool      _myDevice;

  QVariant  _busyLevel;
  QVariant  _callLimit;
  QString   _callerId;
  QString   _callGroup;
  QString   _context;
  QString   _defaultUsername;
  QString   _pickupGroup;
  QString   _regContact;
  QString   _userAgent;
  QVariant  _vmBox;

  bool      _isDndOn;
};

#endif // ASTSIPPEER_H
