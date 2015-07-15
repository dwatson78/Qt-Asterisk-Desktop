#ifndef QTNOTIFY_H
#define QTNOTIFY_H

#if defined EXPORT_LIB
 #define TEST_COMMON_DLLSPEC Q_DECL_EXPORT
#else
 #define TEST_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

#include <QObject>

class QtNotificationPrivate;

namespace QtNotify{
class TEST_COMMON_DLLSPEC QtNotification : public QObject
{
  Q_OBJECT

public:
  explicit QtNotification(QObject *parent = 0);
  ~QtNotification();

  enum Urgency {Low, Normal, Critical};

  // Add an action, Do this for all required actions before running doNotify(...)
  void addAction(const QString &actionName, const QString &labelName);

  // Show the notification
  int doNotify(const QString &appName, const QString &subject, const QString &body, int timeout);

  // Update the notification
  bool updateNotify(const QString &subject, const QString &body, const QString &icon);

Q_SIGNALS:
  void closed(int reasonCode);
  void actionActivated(const QString &actionName);

private:
  QtNotification(const QtNotification &testNotify);
  QtNotification &operator=(const QtNotification &testNotify);

  QtNotificationPrivate *d;
};
}

#endif // QTNOTIFY_H
