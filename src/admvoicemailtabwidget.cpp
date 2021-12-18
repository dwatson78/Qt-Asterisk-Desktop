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

void AdmVoiceMailTabWidget::clearVoiceMailWidgets()
{
  while(ui->_vmTabs->count() > 0)
  {
    QWidget *w = ui->_vmTabs->widget(0);
    ui->_vmTabs->removeTab(0);
    if(NULL != w)
      delete w;
  }
}
