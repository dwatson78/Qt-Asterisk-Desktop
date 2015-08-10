#include "src/dlgphonefeatures.h"
#include "ui_dlgphonefeatures.h"

#include <QDebug>

DlgPhoneFeatures::DlgPhoneFeatures(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DlgPhoneFeatures)
{
  ui->setupUi(this);
}

DlgPhoneFeatures::~DlgPhoneFeatures()
{
  delete ui;
}

void DlgPhoneFeatures::showEvent(QShowEvent *event)
{
  if(event->spontaneous())
    return;
  ui->webView->reload();
}
