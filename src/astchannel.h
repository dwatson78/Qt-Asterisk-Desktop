#ifndef ASTCHANNEL_H
#define ASTCHANNEL_H

#include "asteriskmanager.h"
#include <QObject>
#include <QTime>
#include <QVariantMap>

class ChanPart : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString  Prefix  READ getPrefix   WRITE setPrefix   NOTIFY PrefixChanged)
  Q_PROPERTY(QString  Type  READ getType   WRITE setType   NOTIFY TypeChanged)
  Q_PROPERTY(QString  Exten READ getExten  WRITE setExten  NOTIFY ExtenChanged)
  Q_PROPERTY(QString  Extra READ getExtra  WRITE setExtra  NOTIFY ExtraChanged)
  Q_PROPERTY(QString  Special READ getSpecial  WRITE setSpecial  NOTIFY SpecialChanged)

public:
  explicit ChanPart(QString channel, QObject *parent = 0)
    :QObject(parent)
  {
    _prefix=QString();
    _type=QString();
    _exten=QString();
    _extra=QString();
    _special=QString();
    QRegExp re ("^([^/]+/)?([^/]+)/([^-]+)([-])?([^<]+)?<?([^>]+)?>?$");
    if(re.exactMatch(channel) && re.captureCount() == 6)
    {
      /*for(int i = 0; i <= re.captureCount(); ++i)
      {
        qDebug() << i << ": " << re.cap(i);
      }*/
      this->setPrefix(re.cap(1));
      this->setType(re.cap(2));
      this->setExten(re.cap(3));
      this->setExtra(re.cap(5));
      this->setSpecial(re.cap(6));
      /*qDebug() << tr("ChannelParts: Type: '%1', Exten: '%2', Extra: '%3', Prefix: '%4', Special: '%5'")
                  .arg(this->getType())
                  .arg(this->getExten())
                  .arg(this->getExtra())
                  .arg(this->getPrefix())
                  .arg(this->getSpecial())
      ;*/
    }
  }

  QString getPrefix(){return _prefix;}
  void setPrefix(QString prefix){_prefix = prefix.replace("/","");}

  QString getType(){return _type;}
  void setType(QString type){_type = type;}

  QString getExten(){return _exten;}
  void setExten(QString exten){_exten = exten;}

  QString getExtra(){return _extra;}
  void setExtra(QString extra){_extra = extra;}

  QString getSpecial(){return _special;}
  void setSpecial(QString special){_special = special.replace("<","");}

protected:

signals:
  void PrefixChanged(const QString &newText);
  void TypeChanged(const QString &newText);
  void ExtenChanged(const QString &newText);
  void ExtraChanged(const QString &newText);
  void SpecialChanged(const QString &newText);

private:
  QString _prefix;
  QString _type;
  QString _exten;
  QString _extra;
  QString _special;
};

class AstChannel : public QObject
{
  Q_OBJECT
public:
  explicit AstChannel(QVariantMap &event, QObject *parent = 0);
  ~AstChannel();
  QString getUuid(){return QString(_uuid->toLatin1());}

  QString getCallIdStr();

  QString getConnectedLineStr();

  QString getChannel(){return _channel.isNull() ? "" : _channel;}
  ChanPart* getChannelParts();

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
  const QString* _uuid;
  QTime     *_time;

  QString   _accountCode;
  QString   _callIdName;
  QString   _callIdNum;
  QString   _channel;
  QVariant  _chanState;
  QString   _chanStateDesc;
  QString   _context;
  QVariant  _exten;
  QString   _connectedLineName;
  QVariant  _connectedLineNum;
  QVariant  _hangupCauseNum;
  QString   _hangupCauseDesc;
  QVariant  _callIdPresNum;
  QString   _callIdPresDesc;
  bool _isParked;
  QVariant  _parkedExten;
  QString   _parkedFromName;
  QVariant  _parkedFromNum;
  QString   _parkedFromChannelName;
  bool _isMusicOn;
  AstChannel *_masq;
};

#endif // ASTCHANNEL_H
