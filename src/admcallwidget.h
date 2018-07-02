#ifndef ADMCALLWIDGET_H
#define ADMCALLWIDGET_H

#include <QFrame>
#include "astchannel.h"
#include "admchanwidget.h"
#include "admextensionwidget.h"

namespace Ui {
  class AdmCallWidget;
}

class AdmCallWidget : public QFrame
{
  Q_OBJECT

public:
  explicit AdmCallWidget(QString uuid = QString(), QFrame *parent = 0);
  ~AdmCallWidget();

  QString getUuid(){return _uuid;}
  void addChannel(AstChannel* channel);
  AstChannel * getChannelForDevice(QString type, QString exten, bool *found);
  QMap<QString,AstChannel*> * getChannels(){return _channels;}

public slots:
  void sTickTock();
  void sUpdateChannel(AstChannel* channel);
  void sHangupChannel(AstChannel* channel);
  void sRemoveChannel(AstChannel* channel);

  void sStartCallXfer(AdmExtensionWidget  *widget);
  void sStartCallPark();
  void sStartCallHangup();

protected:

signals:
  void destroying(AdmCallWidget*);
  void callXfer(AdmCallWidget *, const QString &);
  void callPark(AdmCallWidget *);
  void callHangup(AdmCallWidget *);

private:
  Ui::AdmCallWidget *ui;
  QString           _uuid;
  QTime             *_time;

  QMap<QString,AstChannel*>     *_channels;
  QMap<QString,AdmChanWidget*>  *_channelWidgets;
  bool _isDoorIntercom;
};

#endif // ADMCALLWIDGET_H
