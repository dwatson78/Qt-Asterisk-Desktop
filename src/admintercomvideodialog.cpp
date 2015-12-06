#include "admintercomvideodialog.h"
#include "ui_admintercomvideodialog.h"

AdmIntercomVideoDialog::AdmIntercomVideoDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AdmIntercomVideoDialog)
{
  ui->setupUi(this);
  ui->_video->play(QString("rtsp://10.121.212.23:544/live.sdp").data());
}

AdmIntercomVideoDialog::~AdmIntercomVideoDialog()
{
  delete ui;
}
