#include "admintercomvideodialog.h"
#include "ui_admintercomvideodialog.h"
#include "qtasteriskdesktopmain.h"

#include <QUrl>
#include <QTimer>
#include <QDebug>

AdmIntercomVideoDialog::AdmIntercomVideoDialog(QWidget *parent, AstChannel *chan, QUrl url, QString dtmfSequence):
  QDialog(parent),
  ui(new Ui::AdmIntercomVideoDialog)
{
  ui->setupUi(this);
  _chan = chan;
  _url = url;
  _dtmfSequence = dtmfSequence;
  
  connect(ui->_buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this,           SLOT(SBtnClicked(QAbstractButton*))
  );
  connect(ui->_refreshVideo,  SIGNAL(clicked()),
          this,               SLOT(SRefreshVideo())
  );

  if(NULL != chan)
  {
    connect(_chan,  SIGNAL(hangup(AstChannel*)),
            this,     SLOT(sHangupChannel(AstChannel *))
    );
    connect(_chan,  SIGNAL(destroying(AstChannel *)),
            this,     SLOT(sRemoveChannel(AstChannel *))
    );
  }

  if(_dtmfSequence.isNull() || _dtmfSequence.isEmpty() || NULL == _chan)
  {
    ui->_openDoor->setVisible(false);
  } else if(NULL != _chan) {
    connect(ui->_openDoor,  SIGNAL(clicked()),
            this,           SLOT(SOpenDoor())
    );
  }

 _mObj = new Phonon::MediaObject();
 _mSrc = Phonon::MediaSource(_url);
 
 SRefreshVideo();
}

AdmIntercomVideoDialog::~AdmIntercomVideoDialog()
{
  SStop();
  delete ui;
}

void AdmIntercomVideoDialog::SBtnClicked(QAbstractButton* btn)
{
  Q_UNUSED(btn);
  qDebug() << "AdmIntercomVideoDialog::SBtnClicked";
  SStop();
}

void AdmIntercomVideoDialog::SRefreshVideo()
{
  int delay = 250;
  if(ui->_video->isPlaying())
  {
    delay = 2500;
    SStop();
  }
  QTimer::singleShot(delay,this,SLOT(SStart()));
}

void AdmIntercomVideoDialog::SOpenDoor()
{
  if(NULL == _chan)
    return;

  ui->_openDoor->setEnabled(false);
  QtAsteriskDesktopMain::getInstance()->sSendDtmf(_chan, _dtmfSequence, true, 3250);
  QTimer::singleShot(5000,this,SLOT(sEnableOpenDoor()));
}

void AdmIntercomVideoDialog::sEnableOpenDoor()
{
  ui->_openDoor->setEnabled(true);
}

void AdmIntercomVideoDialog::SStop()
{
  if(ui->_video->isPlaying())
  {
    qDebug() << "stop";
    ui->_video->stop();
  }
  ui->_video->load(QUrl());
}

void AdmIntercomVideoDialog::SStart()
{
  qDebug() << "play";
  ui->_video->load(_mSrc);
  ui->_video->play();
}

void AdmIntercomVideoDialog::sHangupChannel(AstChannel *)
{
  QTimer::singleShot(1000,this,SLOT(deleteLater()));
  //this->deleteLater();
}

void AdmIntercomVideoDialog::sRemoveChannel(AstChannel *)
{
  QTimer::singleShot(1000,this,SLOT(deleteLater()));
  //this->deleteLater();
}
