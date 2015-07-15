#include "admnotificationmanager.h"

#include "qtasteriskdesktopmain.h"

#include <QDebug>
AdmNotificationManager::AdmNotificationManager(QObject *parent) :
  QObject(parent)
{
}

void AdmNotificationManager::sNotificationClosed(int reasonCode)
{
  qDebug() << "AdmStatic::sTestNotifyClosed" << reasonCode;
}

void AdmNotificationManager::sNotificationAction(const QString &actionName)
{
  qDebug() << "AdmStatic::sTestNotifyAction" << actionName;

  if(actionName == "Show")
  {
    QtAsteriskDesktopMain::getInstance()->raise();
    QtAsteriskDesktopMain::getInstance()->activateWindow();
  }
}

void AdmNotificationManager::showMsg(const QString &subject, const QString &body, int timeout, QObject *parent)
{
  QtNotify::QtNotification *n = new QtNotify::QtNotification(parent);
  n->doNotify("QtAsteriskDesktop",subject,body,timeout);
  n->deleteLater();
}
