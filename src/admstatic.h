#ifndef ADMSTATIC_H
#define ADMSTATIC_H

#include <QObject>
#include <QTime>
#include <QTimer>

class AdmStatic : public QObject
{
  Q_OBJECT
public:
  /* Public static members */
  static AdmStatic *getInstance();
  static QString elapsedTimeToString(QTime *time);

  /* Public members */
  QTimer* getTimer();
  ~AdmStatic();

public slots:

protected:

signals:

private:
  explicit AdmStatic(QObject *parent = 0);

  /* Private members */
  QTimer      *_timer;

  /* Private static members */
  static  bool        _instanceFlag;
  static  AdmStatic   *_instance;
};



#endif // ADMSTATIC_H
