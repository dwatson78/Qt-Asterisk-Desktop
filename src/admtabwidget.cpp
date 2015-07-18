#include "admtabwidget.h"

#include <QDebug>

AdmTabWidget::AdmTabWidget(QWidget *parent) :
  QTabWidget(parent)
{
  m_tabBar = new AdmTabBar(this);
  setTabBar(m_tabBar);
}

AdmTabWidget::~AdmTabWidget()
{
  if(m_tabBar != NULL)
    delete m_tabBar;
}

void AdmTabWidget::setTabNeedAttention(int tabIndex, bool bold)
{
  m_tabBar->setTabNeedAttention(tabIndex, bold);
}
