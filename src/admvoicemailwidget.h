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

#include <QMenu>
#include <QAction>

#include "phonon/mediaobject.h"
#include "phonon/mediasource.h"
#include "phonon/audiooutput.h"

namespace Ui {
class AdmVoiceMailWidget;
}

class AdmVoiceMailWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit AdmVoiceMailWidget(QString mailbox, QWidget *parent = 0);
  ~AdmVoiceMailWidget();

  const QString &getMailbox(){return _mailbox;}
  const QString &getVmBox(){return _vmBox;}
  const QString &getContext(){return _vmContext;}

public slots:
  void sRefreshView();
  void sVmCountReady(const QVariantMap &data);
  void sVmCountError(QNetworkReply::NetworkError err, const QString &errString);

  void sVmMsgDetailsReady(const QVariantMap &data);
  void sVmMsgDetailsError(QNetworkReply::NetworkError err, const QString &errString);

  void sFolderItemActivated(QListWidgetItem *current, QListWidgetItem * previous);
  void sLoadFolder(const QString &folder);

  void sMessagesItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
  void sMsgCustomContextMenuRequested(QPoint pos);
  void sMsgCustomContextAction(QAction* action);
  void sVmForwardMessageReady(const QVariantMap &data);
  void sVmForwardMessageError(QNetworkReply::NetworkError err, const QString &errString);
  void sVmDeleteMessageReady(const QVariantMap &data);
  void sVmDeleteMessageError(QNetworkReply::NetworkError err, const QString &errString);

  void sMsgCustomContextMoveMsg(QAction* action);
  void sVmMoveMessageReady(const QVariantMap &data);
  void sVmMoveMessageError(QNetworkReply::NetworkError err, const QString &errString);

  void showNetworkErrorMsg(const QString& functionName, QNetworkReply::NetworkError err, const QString &errString);
  void sSendAmiVoicemailRefresh();

  void sPlayMsgClicked();
  void sMsgSeekBackClick();
  void sMsgSeekForwardClick();
  void sSoundFileReady(const QByteArray &data);
  
  void sMediaObjectTick(qint64 time);
  void sMediaObjectStateChanged(Phonon::State newState, Phonon::State oldState);
  void sMediaObjectSourceChanged(Phonon::MediaSource);
  void sMediaObjectFinished();

  void sPlayOnPhoneClicked();

protected:

signals:
  void sigPlayMsgOnPhone(AdmVoiceMailWidget *obj, const QVariantMap &data);
  void sigSeek(qint64 seekTo);

private:
  void _setFolderItemName(QListWidgetItem *item);

  Ui::AdmVoiceMailWidget *ui;
  QString _mailbox;
  QString _vmContext;
  QString _vmBox;
  Phonon::MediaObject *_mObj;
  Phonon::MediaSource _mSrc;
  Phonon::AudioOutput *_mOut;
  //QTemporaryFile *_mFile;

  QString _selectedFolder;
  int _selectedMessageRow;

  void removeTmpMsgFiles();
};


#endif // ADMVOICEMAILWIDGET_H
