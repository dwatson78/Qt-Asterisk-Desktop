#include "admstatic.h"

#include "qtasteriskdesktopmain.h"

#include <QDebug>

QtAsteriskDesktopMain* QtAsteriskDesktopMain::_instance;

bool AdmStatic::_instanceFlag = false;
AdmStatic *AdmStatic::_instance = NULL;

AdmStatic *AdmStatic::getInstance()
{
  if(_instanceFlag == false)
  {
    _instance = new AdmStatic();
    _instanceFlag = true;
    return _instance;
  } else {
    return _instance;
  }
}

QTimer* AdmStatic::getTimer()
{
  return this->_timer;
}
QNetworkAccessManager* AdmStatic::getNetAccessMgr()
{
  return this->_nam;
}
AdmNotificationManager* AdmStatic::getNotificationManager()
{
  return this->_notifMgr;
}

AdmStatic::AdmStatic(QObject *parent) :
  QObject(parent)
{
  _timer = new QTimer();
  _timer->start(500);

  _nam = new QNetworkAccessManager();

  _notifMgr = new AdmNotificationManager(this);
}
AdmStatic::~AdmStatic()
{
  _timer->blockSignals(true);
  _timer->stop();
  _timer->deleteLater();
  _timer = NULL;

   delete _nam;

   delete _notifMgr;

  _instanceFlag = false;
  _instance = NULL;
}

QString AdmStatic::elapsedTimeToString(QTime *time)
{
  int _elapsed = time->elapsed();
  int tsecs = _elapsed / 1000;
  int secs  = tsecs % 60;
  int mins  = (tsecs % 3600) / 60;
  int hours = tsecs / 3600;
  QString elapsStr;
  if(hours == 0)
  {
    elapsStr = tr("%1:%2")
        .arg(mins,2,10,QLatin1Char('0'))
        .arg(secs,2,10,QLatin1Char('0'))
        ;
  } else {
    elapsStr = tr("%1:%2:%3")
        .arg(hours)
        .arg(mins,2,10,QLatin1Char('0'))
        .arg(secs,2,10,QLatin1Char('0'))
        ;
  }
  return elapsStr;
}

QString AdmStatic::extStatusToString(uint extStatusNum)
{
  AsteriskManager::ExtStatuses statuses(extStatusNum);
  QString status;
  if(statuses.testFlag(AsteriskManager::Removed))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("Removed");
  if(statuses.testFlag(AsteriskManager::Deactivated))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("Deactivated");
  if(statuses.testFlag(AsteriskManager::NotInUse))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("NotInUse");
  if(statuses.testFlag(AsteriskManager::InUse))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("InUse");
  if(statuses.testFlag(AsteriskManager::Busy))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("Busy");
  if(statuses.testFlag(AsteriskManager::Unavailable))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("Unavailable");
  if(statuses.testFlag(AsteriskManager::Ringing))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("Ringing");
  if(statuses.testFlag(AsteriskManager::OnHold))
    status = QString("%1%2%3").arg(status).arg(status == QString() ? "" : ", ")
        .arg("OnHold");
  return status;
}

QString AdmStatic::eventToString(const QVariantMap &event, const QString &search)
{
  bool matches = search.isEmpty();
  QRegExp re(search);

  QString o;
  for(QVariantMap::const_iterator i = event.begin(); i != event.end(); ++i)
  {
    o.append(QString("%1: %2\n")
            .arg(i.key())
            .arg(i.value().toString())
    );
    if(!re.isEmpty() && !matches)
    {
      if(i.value().toString().contains(re))
        matches = true;
    }
  }
  if(!o.isEmpty())
    o.append("\n");

  return matches ? o : QString();
}
