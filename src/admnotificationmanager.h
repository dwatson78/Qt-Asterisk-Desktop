#ifndef ADMNOTIFICATIONMANAGER_H
#define ADMNOTIFICATIONMANAGER_H

#include <QObject>
#include <qtnotify/qtnotify.h>

#include "astchannel.h"

class AdmNotificationManager : public QObject
{
  Q_OBJECT
public:
  explicit AdmNotificationManager(QObject *parent = 0);

  static void showMsg(const QString &subject, const QString &body, int timeout, QObject *parent);

signals:
  
public slots:
  void sNotificationClosed(int reasonCode);
  void sNotificationAction(const QString &actionName);

private:

};

#endif // ADMNOTIFICATIONMANAGER_H
