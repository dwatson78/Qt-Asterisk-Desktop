#include "delayedamiaction.h"

#include <QTimer>
#include <QUuid>

#include "admstatic.h"

DelayedAmiAction::DelayedAmiAction(QObject *parent, AsteriskManager *ami, int delay) :
  QObject(parent)
{
  _ami = ami;
  _delay = delay;
}
QString DelayedAmiAction::actionPlayDTMF(QString channel, QChar digit, int duration)
{
  if(NULL == _ami)
    return QString();

  _action = "PlayDTMF";
  _actionId = QUuid::createUuid().toString();

  _headers.clear();
  _headers["Channel"] = channel;
  _headers["Digit"] = QString(digit);
  if(duration > 0)
    _headers["Duration"] = duration;

  QTimer::singleShot(_delay,this,SLOT(sTimeout()));
  return _actionId;
}

QString DelayedAmiAction::actionHangup(QString channel, uint cause)
{
  if(NULL == _ami)
    return QString();

  _action = "Hangup";
  _actionId = QUuid::createUuid().toString();

  _headers.clear();
  _headers["Channel"] = channel;
  _ami->insertNotEmpty(&_headers, "Cause", cause);

  QTimer::singleShot(_delay,this,SLOT(sTimeout()));
  return _actionId;
}

void DelayedAmiAction::sTimeout()
{
  _ami->sendAction(_action,_headers,_actionId);
  emit actionSent(_actionId);
  qDebug() << "DelayedAmiAction::sTimeout" << _action << _actionId << AdmStatic::eventToString(_headers);
  this->deleteLater();
}
