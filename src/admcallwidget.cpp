#include "admcallwidget.h"
#include "ui_admcallwidget.h"

AdmCallWidget::AdmCallWidget(QFrame *parent) :
  QFrame(parent),
  ui(new Ui::AdmCallWidget)
{
	ui->setupUi(this);
	ui->_layoutChans->addWidget(new QLabel("Test"));
}

AdmCallWidget::~AdmCallWidget()
{
	delete ui;
}
