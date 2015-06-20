#ifndef ASTCALL_H
#define ASTCALL_H

#include <QWidget>
#include <QMap>
#include <QTime>
#include <QTimer>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "astchannel.h"


class AstCall : public QWidget
{
  Q_OBJECT
public:
  explicit AstCall(QString uuid, QWidget *parent = 0);
  ~AstCall();

  QString getUuid(){return QString(_uuid->toLatin1());}

  void addChannel(AstChannel* channel);
  AstChannel * getChannelForDevice(QString type, QString exten, bool *found);
  QMap<QString,AstChannel*> *getChannels(){return _channels;}
protected:

signals:
  void destroying(AstCall *);

  void callXfer(AstCall *, const QString &);
  void callPark(AstCall *);
  void callHangup(AstCall *);

public slots:
  void sTickTock();
  void sUpdateChannel(AstChannel* channel);
  void sHangupChannel(AstChannel* channel);
  void sRemoveChannel(AstChannel* channel);

  void sStartCallXfer();
  void sStartCallPark();
  void sStartCallHangup();

private:
  const QString* _uuid;

  QMap<QString,AstChannel*> *_channels;
  QMap<QString,QLabel*>     *_channelWidgets;


  QTime   *_time;
  QTimer  *_timer;

  QGridLayout *_lyt;
  QVBoxLayout *_lytChannels;
  QLabel      *_lbl1;

  qint32 _elapsed;

  void _initWidgit();

};

#endif // ASTCALL_H
