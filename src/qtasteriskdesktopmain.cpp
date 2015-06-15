#include "qtasteriskdesktopmain.h"
#include "ui_qtasteriskdesktopmain.h"

#include "qtasteriskdesktopprefs.h"

#include "asteriskmanager.h"
#include "confbridgeuser.h"

#include <QDebug>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>

QtAsteriskDesktopMain::QtAsteriskDesktopMain(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::QtAsteriskDesktopMain)
{
  ui->setupUi(this);
  _loginActionId = QString();
  _statusIcon = new StatusIcon(this);
  ui->statusBar->addPermanentWidget(_statusIcon);

  /*Connect the Menu Actions*/
  connect(ui->actionPreferences, SIGNAL(triggered()), 
          this,                  SLOT(sPreferences())
  );

  _ami = new AsteriskManager(this);
  connect( _ami, SIGNAL(connected()),
           this, SLOT(asteriskConnected())
  );
  connect(_ami, SIGNAL(disconnected()),
          this, SLOT(asteriskDisconnected())
  );
  connect(_ami, SIGNAL(responseSent(AsteriskManager::Response,QVariantMap,QString)),
          this, SLOT(asteriskResponseSent(AsteriskManager::Response,QVariantMap,QString))
  );
  connect(_ami, SIGNAL(eventGenerated(AsteriskManager::Event,QVariantMap)),
          this, SLOT(asteriskEventGenerated(AsteriskManager::Event,QVariantMap))
  );
  QSettings settings;
  settings.beginGroup("AMI");
  if(settings.contains(AmiPref::getName(AmiPref::host)) && settings.contains(AmiPref::getName(AmiPref::port)))
  {
    bool *valid = new bool(false);
    
    QString host = settings.value(AmiPref::getName(AmiPref::host)).toString();
    uint port = settings.value(AmiPref::getName(AmiPref::port)).toUInt(valid);
    if(valid)
      _ami->connectToHost(host, port);
    else
      asteriskDisconnected();
  }
  else
    asteriskDisconnected();
}

QtAsteriskDesktopMain::~QtAsteriskDesktopMain()
{
  delete ui;
}

void QtAsteriskDesktopMain::asteriskConnected()
{
  qDebug() << "Connected!";
  _statusIcon->setConnected(true);
  QSettings settings;
  settings.beginGroup("AMI");
  if(settings.contains(AmiPref::getName(AmiPref::userName)) && settings.contains(AmiPref::getName(AmiPref::secPass)))
    _loginActionId = _ami->actionLogin(settings.value(AmiPref::getName(AmiPref::userName)).toString(),settings.value(AmiPref::getName(AmiPref::secPass)).toString());
  else
    _ami->disconnectFromHost();
}

void QtAsteriskDesktopMain::asteriskConnected(QString arg1)
{
  qDebug() << "Connected!";
  qDebug() << "arg1: " << arg1;
  _statusIcon->setConnected(true);
}

void QtAsteriskDesktopMain::asteriskDisconnected()
{
  qDebug() << "Disonnected!";
  // _statusIcon->setConnected(false);
}

void QtAsteriskDesktopMain::asteriskResponseSent(AsteriskManager::Response arg1, QVariantMap arg2, QString arg3)
{
  QString response;
  switch(arg1)
  {
    case (AsteriskManager::Success):
      response = "Success";
      break;
    case (AsteriskManager::Error):
      response = "Error";
      break;
    case (AsteriskManager::Follows):
      response = "Follows";
      break;
  }
  qDebug() << "arg1: " << response;
  for(QVariantMap::const_iterator iter = arg2.begin();
      iter != arg2.end();
      ++iter)
  {
    qDebug() << "arg2: " << iter.key() << ": " << iter.value();
  }
  qDebug() << "arg3: " << arg3;
  if(arg3 == _loginActionId)
  {
    qDebug() << "This is our login result!!!!\n\n";
    _statusIcon->setLoggedIn(arg1 == AsteriskManager::Success, arg2.value("Message").toString());
  }
}

void QtAsteriskDesktopMain::asteriskEventGenerated(AsteriskManager::Event arg1, QVariantMap arg2)
{
  switch(arg1)
  {
    case AsteriskManager::ConfbridgeEnd:
    case AsteriskManager::ConfbridgeJoin:
    case AsteriskManager::ConfbridgeLeave:
    case AsteriskManager::ConfbridgeStart:
    case AsteriskManager::ConfbridgeTalking:
      for(QVariantMap::const_iterator iter = arg2.begin();
          iter != arg2.end();
          ++iter)
      {
        qDebug() << "arg2: " << iter.key() << ": " << iter.value();
      }
      qDebug() << "End Event Info\n\n";
      break;
    default:
      break;
  }

  switch(arg1)
  {
    case AsteriskManager::ConfbridgeStart:
      this->showConferenceRoom(arg2);
      break;
    case AsteriskManager::ConfbridgeEnd:
      this->hideConferenceRoom(arg2);
      break;
    case AsteriskManager::ConfbridgeJoin:
      this->addConfBridgeUser(arg2);
      break;
    case AsteriskManager::ConfbridgeLeave:
      this->deleteConfBridgeUser(arg2);
      break;
    case AsteriskManager::ConfbridgeTalking:
      this->confBridgeTalking(arg2);
    default:
      break;
  }
}

void QtAsteriskDesktopMain::showConferenceRoom(QVariantMap arg1)
{
  QGroupBox *groupBox = new QGroupBox(ui->_tab2);
  QString conference = QString::number(arg1.value("Conference").toUInt());
  groupBox->setObjectName(tr("_confBridge%1").arg(conference));
  groupBox->setStyleSheet(QString::fromUtf8("border-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"));
  groupBox->setFlat(false);
  groupBox->setCheckable(false);
  groupBox->setTitle(tr("Conference Room: %1").arg(conference));
  QGridLayout *layout = new QGridLayout(ui->_tab2);
  groupBox->setLayout(layout);
  ui->_confRoomLayout->addWidget(groupBox,0,0,1,1,Qt::AlignLeft | Qt::AlignTop);
}

void QtAsteriskDesktopMain::hideConferenceRoom(QVariantMap arg1)
{
  QString conference = QString::number(arg1.value("Conference").toUInt());
  QGroupBox* gp =  findChild<QGroupBox*>(tr("_confBridge%1").arg(conference));
  if(gp != NULL)
  {
    ui->_confRoomLayout->removeWidget(gp);
    gp->deleteLater();
  }
}

void QtAsteriskDesktopMain::addConfBridgeUser(QVariantMap arg1)
{
/*
    arg2:  "CallerIDname" :  QVariant(QString, "Daniel W Work PC")
    arg2:  "CallerIDnum" :  QVariant(uint, 4004)
    arg2:  "Channel" :  QVariant(QString, "SIP/4004-00000012")
    arg2:  "Conference" :  QVariant(uint, 1080)
    arg2:  "Event" :  QVariant(QString, "ConfbridgeJoin")
    arg2:  "Privilege" :  QVariant(QString, "call,all")
    arg2:  "Uniqueid" :  QVariant(QString, "1434151700.44")
*/
  QString conference = QString::number(arg1.value("Conference").toUInt());
  QGroupBox* gp =  findChild<QGroupBox*>(tr("_confBridge%1").arg(conference));
  if(gp != NULL)
  {
    ConfBridgeUser *usr = new ConfBridgeUser(ui->_tab2, _ami, arg1);
    QGridLayout* lyt = (QGridLayout*)gp->layout();
    lyt->addWidget(usr,lyt->rowCount(),0,1,1,(Qt::AlignLeft | Qt::AlignTop));
  }
}

void QtAsteriskDesktopMain::deleteConfBridgeUser(QVariantMap arg1)
{
  /*
    arg2:  "CallerIDname" :  QVariant(QString, "Daniel W Home PC")
    arg2:  "CallerIDnum" :  QVariant(uint, 4005)
    arg2:  "Channel" :  QVariant(QString, "SIP/4005-00000018")
    arg2:  "Conference" :  QVariant(uint, 1080)
    arg2:  "Event" :  QVariant(QString, "ConfbridgeLeave")
    arg2:  "Privilege" :  QVariant(QString, "call,all")
    arg2:  "Uniqueid" :  QVariant(QString, "1434153872.58")
*/
  QString conference = QString::number(arg1.value("Conference").toUInt());
  QGroupBox* gp =  findChild<QGroupBox*>(tr("_confBridge%1").arg(conference));
  if(gp != NULL)
  {
    ConfBridgeUser* usr = findChild<ConfBridgeUser*>(tr("cbu_%1_%2")
                                     .arg(conference)
                                     .arg(arg1.value("Uniqueid").toString())
    );
    if(usr != NULL)
    {
      QGridLayout* lyt = (QGridLayout*)gp->layout();
      lyt->removeWidget(usr);
      usr->deleteLater();
    }
  }
}

void QtAsteriskDesktopMain::confBridgeTalking(QVariantMap arg1)
{
/*
    arg2:  "Channel" :  QVariant(QString, "SIP/4004-00000015")
    arg2:  "Conference" :  QVariant(uint, 1080)
    arg2:  "Event" :  QVariant(QString, "ConfbridgeTalking")
    arg2:  "Privilege" :  QVariant(QString, "call,all")
    arg2:  "TalkingStatus" :  QVariant(QString, "off")
    arg2:  "Uniqueid" :  QVariant(QString, "1434153120.51")
*/
  ConfBridgeUser* usr = findChild<ConfBridgeUser*>(
        tr("cbu_%1_%2")
          .arg(arg1.value("Conference").toString())
          .arg(arg1.value("Uniqueid").toString())
  );
  if(usr != NULL)
  {
    usr->setTalkingStatus(arg1.value("TalkingStatus").toString() == "on");
  }
}

void QtAsteriskDesktopMain::sPreferences()
{
  qDebug() << "sPreferences";
  QtAsteriskDesktopPrefs * prefs = new QtAsteriskDesktopPrefs(this);
  prefs->exec();
}

