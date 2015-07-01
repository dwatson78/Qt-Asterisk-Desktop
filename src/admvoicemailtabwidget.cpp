#include "admvoicemailtabwidget.h"
#include "ui_admvoicemailtabwidget.h"

AdmVoiceMailTabWidget::AdmVoiceMailTabWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmVoiceMailTabWidget)
{
  ui->setupUi(this);
}

AdmVoiceMailTabWidget::~AdmVoiceMailTabWidget()
{
  delete ui;
}

void AdmVoiceMailTabWidget::addVoiceMailWidget(AdmVoiceMailWidget *widget)
{
  ui->_vmTabs->addTab(widget,widget->getVmBox());
}
