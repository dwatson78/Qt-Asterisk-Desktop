#ifndef ADMSTATIC_H
#define ADMSTATIC_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QNetworkAccessManager>

class AdmStatic : public QObject
{
  Q_OBJECT
public:
  /* Public static members */
  static AdmStatic *getInstance();
  static QString elapsedTimeToString(QTime *time);

  /* Public members */
  ~AdmStatic();
  QTimer* getTimer();
  QNetworkAccessManager* getNetAccessMgr();

public slots:

protected:

signals:

private:
  explicit AdmStatic(QObject *parent = 0);

  /* Private members */
  QTimer      *_timer;
  QNetworkAccessManager *_nam;

  /* Private static members */
  static  bool        _instanceFlag;
  static  AdmStatic   *_instance;

};



#endif // ADMSTATIC_H
