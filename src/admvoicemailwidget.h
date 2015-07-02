#ifndef ADMVOICEMAILWIDGET_H
#define ADMVOICEMAILWIDGET_H

#include <QWidget>
#include <QByteArray>
#include <QBuffer>
#include <QVariantMap>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QTime>
#include <QTemporaryFile>

#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/AudioOutput>


//#include <phonon/mediaobject.h>
//#include <phonon/mediasource.h>
//#include <phonon/videowidget.h>

namespace Ui {
class AdmVoiceMailWidget;
}

class AdmVoiceMailWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit AdmVoiceMailWidget(QString vmBox, QWidget *parent = 0);
  ~AdmVoiceMailWidget();

  const QString &getVmBox(){return _vmBox;}

public slots:
  void sVmCountReady(const QVariantMap &data);
  void sVmCountError(QNetworkReply::NetworkError err);

  void sVmMsgDetailsReady(const QVariantMap &data);
  void sVmMsgDetailsError(QNetworkReply::NetworkError err);

  void sFolderItemActivated(QListWidgetItem *current, QListWidgetItem * previous);

  void sMessagesItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

  void sPlayMsgClicked();
  void sMsgSeekBackClick();
  void sMsgSeekForwardClick();
  void sSoundFileReady(const QByteArray &data);
  
  void sMediaObjectTick(qint64 time);
  void sMediaObjectStateChanged(Phonon::State newState, Phonon::State oldState);
  void sMediaObjectSourceChanged(Phonon::MediaSource);
  void sMediaObjectAboutToFinish();

private:
  Ui::AdmVoiceMailWidget *ui;
  QString _vmBox;
  Phonon::MediaObject *_mObj;
  Phonon::MediaSource _mSrc;
  Phonon::AudioOutput *_mOut;
  QTemporaryFile *_mFile;
};


#endif // ADMVOICEMAILWIDGET_H
