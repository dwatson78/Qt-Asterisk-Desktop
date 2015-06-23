#ifndef ASTPARKEDCALL_H
#define ASTPARKEDCALL_H

#include <QWidget>
#include <QGridLayout>
#include <QTime>
#include "astchannel.h"

namespace Ui
{
  class AstParkedCallWidget;
}

class AstParkedCall : public QWidget
{
  Q_OBJECT
public:
  explicit AstParkedCall(AstChannel *chanParked, AstChannel *chanFrom, QVariantMap event, QWidget *parent = 0);
  ~AstParkedCall();

  QString getUuid(){return QString(_uuid->toLatin1());}

  AstChannel * getChannelParked(){return _chanParked;}
  bool getIsParked(){return _isParked;}
  uint getParkedExten(bool *valid);
  QString getParkedFromName();
  uint getParkedFromNum(bool *valid);
  QString getParkedFromChannel();
  QString getParkedFromStr();

public slots:
  void sDestroyChannel(AstChannel * channel);
  void sParkedCallEvent(AsteriskManager::Event eventType, QVariantMap event);
  void sParkTimeOut(QVariantMap event);
  void sParkGiveUp(QVariantMap event);

  void sStartAnswerParkedCall();

  void sTickTock();

protected:

signals:
  void destroying(AstParkedCall *);
  void parkTimeOut(AstParkedCall *, QVariantMap event);
  void parkGiveUp(AstParkedCall *, QVariantMap event);
  void pickUpParkedCall(AstParkedCall *);

private:
  Ui::AstParkedCallWidget *ui;
  QTime   *_time;

  AstChannel *_chanParked;
  AstChannel *_chanFrom;

  const QString* _uuid;
  bool      _isParked;
  QVariant  _parkedExten;
  QString   _parkedFromName;
  QVariant  _parkedFromNum;
  QString   _parkedFromChannelName;
};

#endif // ASTPARKEDCALL_H
