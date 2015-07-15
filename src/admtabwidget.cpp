#include "admtabwidget.h"

#include <QDebug>

AdmTabWidget::AdmTabWidget(QWidget *parent) :
  QTabWidget(parent)
{
  m_tabBar = new AdmTabBar(this);
  setTabBar(m_tabBar);
}

void AdmTabWidget::setTabNeedAttention(int tabIndex, bool bold)
{
  qDebug() << "AdmTabWidget::setTabNeedAttention" << tabIndex << bold;
  m_tabBar->setTabNeedAttention(tabIndex, bold);
}
