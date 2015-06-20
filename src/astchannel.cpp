#include "astchannel.h"

#include <QTimer>

AstChannel::AstChannel(QVariantMap &event, QObject *parent) :
  QObject(parent)
{
  /*QString*/ this->_accountCode        = QString();
  /*bool*/    //this->_isCallIdSet        = false;
  /*QString*/ this->_callIdName         = QString();
  /*uint*/    this->_callIdNum          = QVariant(QVariant::UInt);
  /*QString*/ this->_channel            = QString();
  /*uint*/    this->_chanState          = QVariant(QVariant::UInt);
  /*QString*/ this->_chanStateDesc      = "Down";
  /*QString*/ this->_context            = QString();
  /*uint*/    this->_exten              = QVariant(QVariant::UInt);
  /*bool*/    //this->_isConnectedLineSet = false;
  /*QString*/ this->_connectedLineName  = QString();
  /*uint*/    this->_connectedLineNum   = QVariant(QVariant::UInt);
  /*bool*/    //this->_isHangupCauseSet   = false;
  /*uint*/    this->_hangupCauseNum     = QVariant(QVariant::UInt);
  /*QString*/ this->_hangupCauseDesc    = QString();
  /*bool*/    //this->_isCallIdPresSet    = false;
  /*uint*/    this->_callIdPresNum      = QVariant(QVariant::UInt);
  /*QString*/ this->_callIdPresDesc     = QString();

  this->_isParked = false;
  this->_parkedExten            = QVariant(QVariant::UInt);
  this->_parkedFromName         = QString();
  this->_parkedFromNum          = QString();
  this->_parkedFromChannelName  = QString();

  this->_isMusicOn              = false;
  this->_masq                   = NULL;

  if(event.contains("Uniqueid"))
  {
    this->_uuid = new QString(event.value("Uniqueid").toString());
    if(event.contains("AccountCode"))
      this->_accountCode = event.value("AccountCode").toString();
    if(event.contains("CallerIDName"))
      this->_callIdName = event.value("CallerIDName").toString();
    if(event.contains("CallerIDNum"))
    {
      this->_callIdNum = event.value("CallerIDNum").toString() == ""
        ? QVariant(QVariant::UInt)
        : event.value("CallerIDNum").toUInt();
    }
    if(event.contains("Channel"))
    {
      this->_channel = event.value("Channel").toString();
      ChanPart *chanPart = getChannelParts();
      if(chanPart->getPrefix() == "Parked")
        this->_isParked = true;
    }
    if(event.contains("ChannelState"))
    {
      this->_chanState = event.value("ChannelState").toString() == ""
        ? QVariant(QVariant::UInt)
        : event.value("ChannelState").toUInt();
    }
    if(event.contains("ChannelStateDesc"))
      this->_chanStateDesc = event.value("ChannelStateDesc").toString();
    if(event.contains("Context"))
      this->_context = event.value("Context").toString();
    if(event.contains("Exten"))
    {
      this->_exten = event.value("Exten").toString() == ""
        ? QVariant(QVariant::UInt)
        : event.value("Exten").toUInt();
    }
    qDebug() << "New Channel: '" << this->_channel << "', Uniqueid: '" << QString(this->_uuid->toLatin1()) << "'";
    if(_isParked)
    {
      emit updated(this);
      emit parkOn(this, event);
    }
  }
  else
    this->deleteLater();
}
AstChannel::~AstChannel()
{
  emit destroying(this);
}

void AstChannel::sChannelEvent(AsteriskManager::Event eventType, QVariantMap event)
{
  switch(eventType)
  {
    case AsteriskManager::Newstate:
    {
      if(event.contains("CallerIDName"))
        this->_callIdName = event.value("CallerIDName").toString();
      if(event.contains("CallerIDNum"))
      {
        this->_callIdNum = event.value("CallerIDNum").toString() == ""
          ? QVariant(QVariant::UInt)
          : event.value("CallerIDNum").toUInt();
      }
      if(event.contains("Channel"))
        this->_channel = event.value("Channel").toString();
      if(event.contains("ChannelState"))
      {
        this->_chanState = event.value("ChannelState").toString() == ""
          ? QVariant(QVariant::UInt)
          : event.value("ChannelState").toUInt();
      }
      if(event.contains("ChannelStateDesc"))
        this->_chanStateDesc = event.value("ChannelStateDesc").toString();
      if(event.contains("ConnectedLineName"))
        this->_connectedLineName = event.value("ConnectedLineName").toString();
      if(event.contains("ConnectedLineNum"))
      {
        this->_connectedLineNum = event.value("ConnectedLineNum").toString() == ""
          ? QVariant(QVariant::UInt)
          : event.value("ConnectedLineNum").toUInt();
      }
      qDebug() << tr("Newstate: Channel: '%1', State: '%2', CallId: '%3', Line: '%4'")
                  .arg(this->_channel)
                  .arg(this->_chanStateDesc)
                  .arg(this->getCallIdStr())
                  .arg(this->getConnectedLineStr())
                  ;
      ChanPart *p = this->getChannelParts();
      emit updated(this);
      break;
    }
    case AsteriskManager::NewCallerid:
    {
      if(event.contains("Channel"))
        this->_channel = event.value("Channel").toString();
      if(event.contains("CallerIDName"))
        this->_callIdName = event.value("CallerIDName").toString();
      if(event.contains("CallerIDNum"))
      {
        this->_callIdNum = event.value("CallerIDNum").toString() == ""
          ? QVariant(QVariant::UInt)
          : event.value("CallerIDNum").toUInt();
      }
      if(event.contains("CID-CallingPres"))
      {
        QRegExp re ("^([0-9]+)\\ \\((.*)\\)$");
        if(re.exactMatch(event.value("CID-CallingPres").toString()))
        {
          QStringList caps = re.capturedTexts();
          this->_callIdPresNum = caps.at(1).toUInt();
          this->_callIdPresDesc = caps.at(2);
        }
      }
      ChanPart *p = this->getChannelParts();
      emit updated(this);
      break;
    }
    case AsteriskManager::Rename:
    {
      qDebug() << "AstChannel::sChannelEvent: Rename";
      if(event.contains("Newname"))
      {
        qDebug() << tr("Rename '%1' to '%2")
                    .arg(this->_channel)
                    .arg(event.value("Newname").toString())
        ;
        this->_channel = event.value("Newname").toString();
        emit updated(this);
      }
      break;
    }
    case AsteriskManager::ParkedCall:
    {
      qDebug() << tr("AstChannel::sChannelEvent: ParkedCall: Channel: '%1', uuid: '%2'")
                  .arg(this->_channel)
                  .arg(QString(this->_uuid->toLatin1()));
      this->sParkOn(event);
      if(NULL != _masq)
      {
        _masq->sParkOn(event);
      }
      break;
    }
    case AsteriskManager::UnParkedCall:
    case AsteriskManager::ParkedCallGiveUp:
    case AsteriskManager::ParkedCallTimeOut:
    {
      qDebug() << tr("AstChannel::sChannelEvent: UnParkedCall: Channel: '%1', uuid: '%2'")
                  .arg(this->_channel)
                  .arg(QString(this->_uuid->toLatin1()));
      this->sParkOff(event);
      if(NULL != _masq)
      {
        _masq->sParkOff(event);
      }
      break;
    }
    case AsteriskManager::MusicOnHold:
    {
      qDebug() << tr("AstChannel::sChannelEvent: MusicOnHold: Channel: '%1', uuid: '%2', State: '%3'")
                  .arg(this->_channel)
                  .arg(QString(this->_uuid->toLatin1()))
                  .arg(event.contains("State")
                       ? event.value("State").toString()
                       : "")
      ;
      if(event.contains("State"))
      {
        if(event.value("State").toString() == "Start")
        {
          this->sMusicOn(event);
          if(NULL != this->_masq)
            this->_masq->sMusicOn(event);
        }
        else if(event.value("State").toString() == "Stop")
        {
          this->sMusicOff(event);
          if(NULL != this->_masq)
            this->_masq->sMusicOff(event);
        }
      }
      break;
    }

    case AsteriskManager::Hangup:
    {
      if(event.contains("AccountCode"))
        this->_accountCode = event.value("AccountCode").toString();
      if(!_isParked && event.contains("CallerIDName"))
        this->_callIdName = event.value("CallerIDName").toString();
      if(!_isParked && event.contains("CallerIDNum"))
        this->_callIdNum = event.value("CallerIDNum").toUInt();
      if(event.contains("Cause"))
        this->_hangupCauseNum = event.value("Cause").toUInt();
      if(event.contains("Cause-txt"))
        this->_hangupCauseDesc = event.value("Cause-txt").toString();
      if(event.contains("Channel"))
        this->_channel = event.value("Channel").toString();
      if(event.contains("ConnectedLineName"))
        this->_connectedLineName = event.value("ConnectedLineName").toString();
      if(event.contains("ConnectedLineNum"))
        this->_connectedLineNum = event.value("ConnectedLineNum").toUInt();
      qDebug() << tr("Newstate: Channel: '%1', State: '%2', CallId: '%3', Line: '%4', HangupCause: '%5', '%6'")
                  .arg(this->_channel)
                  .arg(this->_chanStateDesc)
                  .arg(this->getCallIdStr())
                  .arg(this->getConnectedLineStr())
                  .arg(this->_hangupCauseDesc)
                  .arg(this->_hangupCauseNum.isNull() ? "" : this->_hangupCauseNum.toString())
                  ;
      ChanPart *p = this->getChannelParts();
      emit updated(this);
      emit hangup(this);
      QTimer::singleShot(5000, this, SLOT(deleteLater()));
      break;
    }
    default:
      break;
  }
}

QString AstChannel::getCallIdStr()
{
  if(!this->_callIdName.isNull() && !this->_callIdNum.isNull())
    return tr("%1 <%2>").arg(this->_callIdName).arg(this->_callIdNum.toString());
  else if(!this->_callIdName.isNull() && this->_callIdNum.isNull())
    return this->_callIdName;
  else if(this->_callIdName.isNull() && !this->_callIdNum.isNull())
    return this->_callIdNum.toString();
  else
    return "";
}

QString AstChannel::getConnectedLineStr()
{
  if(!this->_connectedLineName.isNull() && !this->_connectedLineNum.isNull())
    return tr("%1 <%2>").arg(this->_connectedLineName).arg(this->_connectedLineNum.toString());
  else if(!this->_connectedLineName.isNull() && this->_connectedLineNum.isNull())
    return this->_connectedLineName;
  else if(this->_connectedLineName.isNull() && !this->_connectedLineNum.isNull())
    return this->_connectedLineNum.toString();
  else
    return "";
}

ChanPart * AstChannel::getChannelParts()
{
  return new ChanPart(this->getChannel());
}

uint AstChannel::getChannelState(bool *valid)
{
  if(this->_chanState.isNull())
  {
    *valid = false;
    return 0;
  } else {
    *valid = true;
    return this->_chanState.toUInt(valid);
  }
}

QString AstChannel::getChannelStateDesc()
{
  return this->_chanStateDesc.isNull() ? "" : this->_chanStateDesc;
}

QString AstChannel::getChannelStateStr()
{
  if(!this->_chanStateDesc.isNull() && !this->_chanState.isNull())
    return tr("%1 (%2)>").arg(this->_chanState.toString()).arg(this->_chanStateDesc);
  else if(!this->_chanStateDesc.isNull() && this->_chanState.isNull())
    return this->_chanStateDesc;
  else if(this->_chanStateDesc.isNull() && !this->_chanState.isNull())
    return this->_chanState.toString();
  else
    return "";
}

QString AstChannel::getContext()
{
  return this->_context.isNull() ? "" : this->_context;
}

uint    AstChannel::getExten(bool *valid)
{
  if(this->_exten.isNull())
  {
    *valid = false;
    return 0;
  } else {
    *valid = true;
    return _exten.toUInt(valid);
  }
}

uint    AstChannel::getHangupCauseNum(bool *valid)
{
  if(this->_hangupCauseNum.isNull())
  {
    *valid = false;
    return 0;
  } else {
    *valid = true;
    return _hangupCauseNum.toUInt(valid);
  }
}
QString AstChannel::getHangupCauseDesc()
{
  return this->_hangupCauseDesc.isNull() ? "" : this->_hangupCauseDesc;

}
QString AstChannel::getHangupCauseStr()
{
  if(!this->_hangupCauseDesc.isNull() && !this->_hangupCauseNum.isNull())
    return tr("%1 (%2)>").arg(this->_hangupCauseNum.toString()).arg(this->_hangupCauseDesc);
  else if(!this->_hangupCauseDesc.isNull() && this->_hangupCauseNum.isNull())
    return this->_hangupCauseDesc;
  else if(this->_hangupCauseDesc.isNull() && !this->_hangupCauseNum.isNull())
    return this->_hangupCauseNum.toString();
  else
    return "";
}

void AstChannel::sParkOn(QVariantMap event)
{
  /*
    catchMeIfYouCan: Begin Caught Event Info
    catchMeIfYouCan: arg1: Event:  ParkedCall
    catchMeIfYouCan: arg2:  "CallerIDName" :  QVariant(QString, "Daniel W Home PC")
    catchMeIfYouCan: arg2:  "CallerIDNum" :  QVariant(uint, 4005)
    catchMeIfYouCan: ChanVar:  "SIP/4005-0000023f" :  "CHANNEL(uniqueid)" :  QVariant(QString, "1434668609.692")
    Test Static:  "1434668609.692"
    catchMeIfYouCan: arg2:  "Channel" :  QVariant(QString, "SIP/4005-0000023f")
    catchMeIfYouCan: arg2:  "ConnectedLineName" :  QVariant(QString, "Daniel W Work PC")
    catchMeIfYouCan: arg2:  "ConnectedLineNum" :  QVariant(uint, 4004)
    catchMeIfYouCan: arg2:  "Event" :  QVariant(QString, "ParkedCall")
    catchMeIfYouCan: arg2:  "Exten" :  QVariant(uint, 701)
    catchMeIfYouCan: arg2:  "From" :  QVariant(QString, "SIP/4004-0000023e")
    catchMeIfYouCan: arg2:  "Parkinglot" :  QVariant(QString, "default")
    catchMeIfYouCan: arg2:  "Privilege" :  QVariant(QString, "call,all")
    catchMeIfYouCan: arg2:  "Timeout" :  QVariant(uint, 45)
    catchMeIfYouCan: arg2:  "Uniqueid" :  QVariant(QString, "1434668609.692")
    catchMeIfYouCan: End Caught Event Info
  */
  this->_isParked = true;
  if(event.contains("Exten"))
    _parkedExten = event.value("Exten").toUInt();
  if(event.contains("ConnectedLineName"))
    _parkedFromName = event.value("ConnectedLineName").toString();
  if(event.contains("ConnectedLineNum"))
    _parkedFromNum = event.value("ConnectedLineNum").toString();
  if(event.contains("From"))
    _parkedFromChannelName = event.value("From").toString();
  emit updated(this);
  emit parkOn(this, event);
}
void AstChannel::sParkOff(QVariantMap event)
{
  this->_isParked               = false;
  this->_parkedExten            = QVariant(QVariant::UInt);
  this->_parkedFromName         = QString();
  this->_parkedFromNum          = QString();
  this->_parkedFromChannelName  = QString();
  emit updated(this);
  emit parkOff(this, event);
}


void AstChannel::sMusicOn(QVariantMap event)
{
  this->_isMusicOn = true;
  emit updated(this);
  emit musicOn(this, event);
}
void AstChannel::sMusicOff(QVariantMap event)
{
  this->_isMusicOn = false;
  emit updated(this);
  emit musicOff(this, event);
}


QVariant AstChannel::getChanVar(QVariantMap event, QString channel, QString variable, bool *found)
{
  *found = true;
  if(event.contains("ChanVars"))
  {
    QVariantMap chanVars = event.value("ChanVars").toMap();
    if(chanVars.contains(channel))
    {
      QVariantMap chanVarVals = chanVars.value(channel).toMap();
      if(chanVarVals.contains(variable))
        return chanVarVals.value(variable);
    }
  }
  *found = false;
  return QVariant();
}

void AstChannel::sMasqueradeChannel(QVariantMap event, AstChannel *clone)
{
  qDebug() << event.value("Event").toString();
  this->_accountCode        = QString(clone->_accountCode);
  this->_callIdName         = QString(clone->_callIdName);
  //this->_channel            = QString(clone->_channel);
  this->_chanStateDesc      = QString(clone->_chanStateDesc);
  this->_context            = QString(clone->_context);
  this->_connectedLineName  = QString(clone->_connectedLineName);
  this->_hangupCauseDesc    = QString(clone->_hangupCauseDesc);
  this->_callIdPresDesc     = QString(clone->_callIdPresDesc);

  this->_callIdNum        = QVariant(clone->_callIdNum);
  this->_chanState        = QVariant(clone->_chanState);
  this->_exten            = QVariant(clone->_exten);
  this->_connectedLineNum = QVariant(clone->_connectedLineNum);
  this->_hangupCauseNum   = QVariant(clone->_hangupCauseNum);
  this->_callIdPresNum    = QVariant(clone->_callIdPresNum);

  this->_isMusicOn = clone->_isMusicOn;
  _masq = clone;
  connect(_masq,  SIGNAL(destroying(AstChannel*)),
          this,   SLOT(sDestroyingMasq(AstChannel*))
  );
  //this->_isParked = clone->_isParked;
  emit updated(this);
}

void AstChannel::sDestroyingMasq(AstChannel *masq)
{
  if(_masq == NULL)
    return;
  else
    _masq = NULL;
  qDebug() << tr("Removed masq: uuid: '%1', channel: '%2'")
            .arg(masq->getUuid())
           .arg(masq->getChannel())
  ;
}
uint AstChannel::getParkedExten(bool *valid)
{
  if(!this->_parkedExten.isNull())
  {
    *valid = true;
    return this->_parkedExten.toUInt();
  }
  *valid = false;
  return 0;
}

QString AstChannel::getParkedFromName()
{
  return this->_parkedFromName;
}

uint AstChannel::getParkedFromNum(bool *valid)
{
  if(!this->_parkedFromNum.isNull())
  {
    *valid = true;
    return this->_parkedFromNum.toUInt();
  }
  *valid = false;
  return 0;
}

QString AstChannel::getParkedFromChannel()
{
  return this->_parkedFromChannelName;

}

QString AstChannel::getParkedFromStr()
{
  if(_isParked)
  {
    if(!this->_parkedFromNum.isNull())
    {
      return QString("%1 <%$2>").arg(this->_parkedFromName).arg(this->_parkedFromNum.toString());
    } else {
      return QString("%1").arg(this->_parkedFromName);
    }
  }
  return QString();
}
