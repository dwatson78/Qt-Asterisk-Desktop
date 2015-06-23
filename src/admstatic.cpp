#include "admstatic.h"

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

AdmStatic::AdmStatic(QObject *parent) :
  QObject(parent)
{
  _timer = new QTimer();
  _timer->start(500);
}
AdmStatic::~AdmStatic()
{
  _timer->blockSignals(true);
  _timer->stop();
  _timer->deleteLater();
  _timer = NULL;
  _instanceFlag = false;
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
