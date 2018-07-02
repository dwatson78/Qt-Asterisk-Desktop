#ifndef DELAYEDAMIACTION_H
#define DELAYEDAMIACTION_H

#include <QObject>
#include "asteriskmanager.h"

class DelayedAmiAction : public QObject
{
  Q_OBJECT

public:
  explicit DelayedAmiAction(QObject *parent = 0, AsteriskManager *ami = 0, int delay = 100);

  QString actionPlayDTMF(QString channel, QChar digit = QChar(), int duration = 0);
  QString actionHangup(QString channel, uint cause = 0);

signals:
  void actionSent(QString);

public slots:
  void sTimeout();

private:
  AsteriskManager *_ami;
  int             _delay;
  QVariantMap     _headers;
  QString         _action;
  QString         _actionId;
};

#endif // DELAYEDAMIACTION_H
