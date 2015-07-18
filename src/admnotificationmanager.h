#ifndef ADMNOTIFICATIONMANAGER_H
#define ADMNOTIFICATIONMANAGER_H

#include <QObject>
#include "qt-wrap-libnotify/qtnotify.h"

#include "astchannel.h"

class AdmNotificationManager : public QObject
{
  Q_OBJECT
public:
  explicit AdmNotificationManager(QObject *parent = 0);
  ~AdmNotificationManager();

  static void showMsg(const QString &subject, const QString &body, const QString &icon, int timeout, QObject *parent);

  void startCallNotification(AstChannel *chan);
  void stopCallNotification(AstChannel *chan);

  virtual void timerEvent(QTimerEvent *event);

signals:

public slots:
  void sNotificationClosed(int reasonCode);
  void sNotificationAction(const QString &actionName);
  void sChannelDestroying(AstChannel *chan);
  void sChannelUpdated(AstChannel *chan);

private:
  QMap<AstChannel *, QtNotify::QtNotification *> m_call_notifications;
  QMap<int, QtNotify::QtNotification *> m_timers;

};

#endif // ADMNOTIFICATIONMANAGER_H
