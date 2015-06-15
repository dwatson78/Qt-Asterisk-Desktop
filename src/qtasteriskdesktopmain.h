#ifndef QTASTERISKDESKTOPMAIN_H
#define QTASTERISKDESKTOPMAIN_H

#include <QMainWindow>
#include "asteriskmanager.h"
#include "statusicon.h"

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
  void sPreferences();

  void asteriskConnected();
  void asteriskConnected(QString arg1);
  void asteriskDisconnected();
  void asteriskResponseSent(AsteriskManager::Response arg1, QVariantMap arg2, QString arg3);
  void asteriskEventGenerated(AsteriskManager::Event arg1, QVariantMap arg2);

  void showConferenceRoom(QVariantMap arg1);
  void hideConferenceRoom(QVariantMap arg1);
  void addConfBridgeUser(QVariantMap arg1);
  void deleteConfBridgeUser(QVariantMap arg1);
  void confBridgeTalking(QVariantMap arg1);

private:
  Ui::QtAsteriskDesktopMain *ui;
  AsteriskManager *_ami;
  StatusIcon *_statusIcon;
  QString _loginActionId;
};

#endif // QTASTERISKDESKTOPMAIN_H
