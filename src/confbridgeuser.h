#ifndef CONFBRIDGEUSER_H
#define CONFBRIDGEUSER_H

#include "asteriskmanager.h"

#include <QWidget>
#include <QVariantMap>
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>


// TODO: Clean this up!!
class ConfBridgeUser : public QWidget
{
  Q_OBJECT
public:
  explicit ConfBridgeUser(QWidget *parent = 0, AsteriskManager* ami = 0, QVariantMap event = QVariantMap());
  
signals:
  
public slots:
  void setTalkingStatus(bool talking);
  void contextMenuRequested(const QPoint& point);
  void mute();
  void unmute();
  void kick();

private:
  AsteriskManager* _ami;
  uint _conference;
  QString _uuid;
  QString _channel;
  QLabel* _lbl;
  QHBoxLayout* _lyt;

  bool talkingWhileMuted;
  bool muted;
  bool talking;
};

#endif // CONFBRIDGEUSER_H
