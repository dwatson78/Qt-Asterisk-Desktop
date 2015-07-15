#include "admnotificationmanager.h"

#include "qtasteriskdesktopmain.h"

#include <QCoreApplication>
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

void AdmNotificationManager::showMsg(const QString &subject, const QString &body, const QString &icon, int timeout, QObject *parent)
{
  QtNotify::QtNotification *n = new QtNotify::QtNotification(parent);
  n->doNotify(QCoreApplication::applicationName(),subject,body,icon,timeout);
  n->deleteLater();
}

void AdmNotificationManager::startCallNotification(AstChannel *chan)
{
  if(!m_call_notifications.contains(chan))
  {

    QtNotify::QtNotification *n = new QtNotify::QtNotification(chan);

    m_call_notifications.insert(chan,n);
    connect(chan, SIGNAL(destroying(AstChannel*)),
            this, SLOT(sChannelDestroying(AstChannel*)));
    connect(chan, SIGNAL(updated(AstChannel*)),
            this, SLOT(sChannelUpdated(AstChannel*)));

    n->doNotify(QCoreApplication::applicationName(),"Incoming Call",chan->getConnectedLineStr(),"call-start",5000);
  }
}

void AdmNotificationManager::stopCallNotification(AstChannel *chan)
{
  if(m_call_notifications.contains(chan))
  {
    QtNotify::QtNotification *n = m_call_notifications.value(chan);
    if(n)
    {
      disconnect(chan, SIGNAL(destroying(AstChannel*)),
              this, SLOT(sChannelDestroying(AstChannel*)));
      disconnect(chan, SIGNAL(updated(AstChannel*)),
              this, SLOT(sChannelUpdated(AstChannel*)));
      n->deleteLater();
    }
    m_call_notifications.remove(chan);
  }
}

void AdmNotificationManager::sChannelDestroying(AstChannel *chan)
{
  stopCallNotification(chan);
}

void AdmNotificationManager::sChannelUpdated(AstChannel *chan)
{
  QString subject, body, icon;
  bool cancelNotification = false;
  if(m_call_notifications.contains(chan))
  {
    bool ok;
    chan->getHangupCauseNum(&ok);
    if(ok)
    {
       qDebug() << "AdmNotificationManager::sChannelUpdated: Hangup: " << chan->getHangupCauseStr();
       subject = "Call Ended";
       body = chan->getConnectedLineStr()
                .append("\n\n")
                .append(chan->getHangupCauseDesc());
       icon = "call-stop";
    } else {
      //Find out the nature of the channel status
      qDebug() << "AdmNotificationManager::sChannelUpdated: Channel State: " << chan->getChannelStateStr();
      uint chanState = chan->getChannelState(&ok);

      if(ok)
      {
        switch(chanState)
        {
          case 0: // down & available
          case 1: // down but reserved
          case 2: // off hook
          {
            cancelNotification = true;
            break;
          }
          case 3: // digits have been dialed
          case 4: // line is ringing
          case 5: // remote end is ringing
          {
            qWarning() << "Unhandled channel state after ringing to channel state: " << chanState;
            break;
          }
          case 6: // line is up
          {
            subject = "Call Handled";
            body = chan->getConnectedLineStr();
            icon = "dialog-information";
            break;
          }
          case 7: // line is busy
            subject = "Call Busy";
            body = chan->getConnectedLineStr();
            icon = "dialog-error";
          default:
          {
            qWarning() << "Unhandled channel state after ringing to channel state: " << chanState;
            break;
          }
        }
      }
    }

    if(cancelNotification)
    {
      stopCallNotification(chan);
      return;
    }

    if(!subject.isNull() && !body.isNull() && !icon.isNull())
    {
      QtNotify::QtNotification *n = m_call_notifications.value(chan);
      if(n)
      {
        bool updated = n->updateNotify(subject,body,icon);
        qDebug() << "AdmNotificationManager::sChannelUpdated" << updated;
        if(updated)
        {
          // Start a one second timer to close the notification after updating
          qDebug() << "Closing the notification in 2 seconds...";
          int timer = startTimer(2000);
          m_timers.insert(timer,n);
        }
      }
    }
  }
}

void AdmNotificationManager::timerEvent(QTimerEvent *event)
{
  int timerId = event->timerId();
  if(m_timers.contains(timerId))
  {
    QtNotify::QtNotification *n = m_timers.value(timerId);
    if(n)
    {
      // Close the notification
      n->closeNotify();

      // Stop call notifications for this channel
      int index = m_call_notifications.values().indexOf(n);
      if(index > -1)
      {
        AstChannel *chan = m_call_notifications.keys().at(index);
        stopCallNotification(chan);
      }
    }
  }
  killTimer(timerId);
}
