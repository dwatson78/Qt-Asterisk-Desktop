#include "admintercomvideodialog.h"
#include "ui_admintercomvideodialog.h"

#include <QUrl>
#include <QTimer>

AdmIntercomVideoDialog::AdmIntercomVideoDialog(QWidget *parent, QUrl url):
  QDialog(parent),
  ui(new Ui::AdmIntercomVideoDialog)
{
  ui->setupUi(this);
  
  _url = url;
  
  connect(ui->_buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this,           SLOT(SBtnClicked(QAbstractButton*))
  );
  connect(ui->_refreshVideo,  SIGNAL(clicked()),
          this,               SLOT(SRefreshVideo())
  );
  connect(ui->_openDoor,  SIGNAL(clicked()),
          this,           SLOT(SOpenDoor())
  );
  
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
