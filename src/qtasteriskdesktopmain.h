#ifndef QTASTERISKDESKTOPMAIN_H
#define QTASTERISKDESKTOPMAIN_H

#include <QMainWindow>
#include <QMap>
#include <QList>
#include "asteriskmanager.h"
#include "statusicon.h"
#include "astchannel.h"
#include "astparkedcall.h"
#include "admcallwidget.h"
#include "astsippeer.h"
#include "admvoicemailtabwidget.h"
#include "dlgphonefeatures.h"
#include "astlocalbridge.h"

#ifdef AST_DEBUG
  #include <QScopedPointer>
  #include <QFile>
#endif

namespace Ui {
class QtAsteriskDesktopMain;
}

class QtAsteriskDesktopMain : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit QtAsteriskDesktopMain(QWidget *parent = 0);
  ~QtAsteriskDesktopMain();

public slots:
  void sExit();
  void sPreferences();
  void sPhoneFeatures();

  void connectToAsterisk();
  void asteriskConnected();
  void asteriskConnected(QString arg1);
  void asteriskDisconnected();
  void asteriskError(QAbstractSocket::SocketError);
  void asteriskResponseSent(AsteriskManager::Response arg1, QVariantMap arg2, QString arg3);
  void asteriskEventGenerated(AsteriskManager::Event arg1, QVariantMap arg2);

  void showConferenceRoom(QVariantMap arg1);
  void hideConferenceRoom(QVariantMap arg1);
  void addConfBridgeUser(QVariantMap arg1);
  void deleteConfBridgeUser(QVariantMap arg1);
  void confBridgeTalking(QVariantMap arg1);

  void sDial();
  void sSetExtStatus(uint ext, AsteriskManager::ExtStatuses statuses);

  void sCallXfer(AdmCallWidget * call, const QString & exten);
  void sCallPark(AdmCallWidget * call);
  void sCallHangup(AdmCallWidget * call);
  void sDestroyingParkedCalled(AstParkedCall *parkedCall);
  void sPickUpParkedCall(AstParkedCall *parkedCall);
  void sDestroyingChannel(AstChannel *channel);
  void sDestroyingCall(AdmCallWidget *call);
  void sDestroyingSipPeer(AstSipPeer *peer);
  void sDestroyingAdmExtensionWidget(AdmExtensionWidget *widget);

  void sPlayMsgOnPhone(AdmVoiceMailWidget* obj, const QVariantMap &data);

  void sSendAmiVoicemailRefresh(QString mailbox, QString context = "default");

  static QtAsteriskDesktopMain* getInstance(){return _instance;}

private:
  Ui::QtAsteriskDesktopMain *ui;
  AsteriskManager *_ami;
  StatusIcon *_statusIcon;
  QString _loginActionId;
  QString _sipPeersActionId;
  QMap<QString, AstSipPeer *>         * _showSipPeerActionId;
  QMap<QString, AstSipPeer *>         * _extensionStateActionId;
  QMap<QString, AstChannel*>          * _chanMap;
  QMap<QString, AstParkedCall*>       * _parkedMap;
  QMap<QString, AdmCallWidget*>       * _callMap;
  QMap<QString, AdmExtensionWidget *> * _extensionMap;
  QMap<QString, AstSipPeer *>         * _sipPeerMap;
  QMap<QString, AstSipPeer *>         * _mySipPeerMap;
  QList<AstLocalBridge>               * _localBridgeMap;
  static QtAsteriskDesktopMain        * _instance;
  DlgPhoneFeatures                    * _phoneFeatures;
  uint                                  _asteriskConnectInterval;

#ifdef AST_DEBUG
  QScopedPointer<QFile> m_astdebugFile;
  int m_astdebugSequence;
#endif

};

#endif // QTASTERISKDESKTOPMAIN_H
