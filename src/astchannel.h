#ifndef ASTCHANNEL_H
#define ASTCHANNEL_H

#include "asteriskmanager.h"
#include <QObject>
#include <QTime>
#include <QVariantMap>
#include <QSettings>
#include <QList>

#include "astchanparts.h"

class AstChannel : public QObject
{
  Q_OBJECT
public:
  explicit AstChannel(const QString &uuid, QVariantMap &event, QObject *parent = 0);
  ~AstChannel();
  QString getUuid(){return _uuid;}

  QString getCallIdStr();

  QString getConnectedLineStr();

  QString getChannel(){return _channel.isNull() ? "" : _channel;}
  AstChanParts* getChannelParts();

  uint getChannelState(bool *valid);
  QString getChannelStateDesc();
  QString getChannelStateStr();

  QString getContext();

  uint getExten(bool *valid);

  uint    getHangupCauseNum(bool *valid);
  QString getHangupCauseDesc();
  QString getHangupCauseStr();

  bool getIsParked(){return _isParked;}
  uint getParkedExten(bool *valid);
  QString getParkedFromName();
  uint getParkedFromNum(bool *valid);
  QString getParkedFromChannel();
  QString getParkedFromStr();

  bool getIsMusicOn(){return _isMusicOn;}

  static QVariant getChanVar(QVariantMap event, QString channel, QString variable, bool *found);

  QString getBridgeUuid(){return _bridgeUuid;}
  void setBridgeUuid(QString bridgeUuid){_bridgeUuid = bridgeUuid;}

  QList<QVariantMap> getEvents();


public slots:
  void sChannelEvent(AsteriskManager::Event eventType, QVariantMap event);
  void sMasqueradeChannel(QVariantMap event, AstChannel *clone);
  void sDestroyingMasq(AstChannel *masq);
  void sParkOn(QVariantMap event);
  void sParkOff(QVariantMap event);
  void sMusicOn(QVariantMap event);
  void sMusicOff(QVariantMap event);

protected:

signals:
  void updated(AstChannel *);
  void hangup(AstChannel *);
  void destroying(AstChannel *);
  void parkOn(AstChannel *, QVariantMap event);
  void parkOff(AstChannel *, QVariantMap event);
  void musicOn(AstChannel *, QVariantMap event);
  void musicOff(AstChannel *, QVariantMap event);

private:
  const QString   _uuid;
  QTime         * _time;

  QString       _accountCode;
  QString       _callIdName;
  QString       _callIdNum;
  QString       _channel;
  QVariant      _chanState;
  QString       _chanStateDesc;
  QString       _context;
  QVariant      _exten;
  QString       _connectedLineName;
  QString       _connectedLineNum;
  QVariant      _hangupCauseNum;
  QString       _hangupCauseDesc;
  QVariant      _callIdPresNum;
  QString       _callIdPresDesc;
  bool          _isParked;
  QVariant      _parkedExten;
  QString       _parkedFromName;
  QVariant      _parkedFromNum;
  QString       _parkedFromChannelName;
  bool          _isMusicOn;
  AstChannel  * _masq;
  QString       _bridgeUuid;
  QList<QVariantMap> _events;

};

#endif // ASTCHANNEL_H
