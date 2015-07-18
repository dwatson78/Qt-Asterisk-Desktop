#include "admtabbar.h"

#include <QDebug>
#include <QLabel>
#include <QPushButton>

AdmTabBar::AdmTabBar(QWidget *parent) :
  QTabBar(parent)
{
  m_time = NULL;
  m_timer = NULL;
  m_blinkStatus = false;
}
AdmTabBar::~AdmTabBar()
{
  if(m_time != NULL)
    delete m_time;
  if(m_timer != NULL)
    delete m_timer;
}

void AdmTabBar::addBlinkingTab(int tabIndex)
{
  if(m_blinkTabs.contains(tabIndex))
    return;

  m_blinkTabs.append(tabIndex);
  if(NULL == m_time)
  {
    m_time = new QTime();
    m_time->start();
    m_timer = new QTimer();
    connect(m_timer,  SIGNAL(timeout()), 
            this,     SLOT(sTickTock()));
    m_timer->start(125);
  }
}
void AdmTabBar::delBlinkingTab(int tabIndex)
{
  if(m_blinkTabs.contains(tabIndex))
  {
    m_timer->blockSignals(true);
    m_blinkTabs.removeOne(tabIndex);
    setTabTextColor(tabIndex,palette().color(QPalette::WindowText));
    m_timer->blockSignals(false);
  }

  if(m_blinkTabs.count() == 0)
  {
    disconnect(m_timer, SIGNAL(timeout()), 
               this,    SLOT(sTickTock()));
    delete m_timer;
    m_timer = NULL;
    delete m_time;
    m_time = NULL;
  }
}

void AdmTabBar::sTickTock()
{
  if(NULL != m_time && m_time->elapsed() >= 250)
  {
    for(int i = 0; i < m_blinkTabs.count(); ++i)
    {
      if(m_blinkStatus)
        setTabTextColor(m_blinkTabs.at(i),palette().color(QPalette::Link));
      else
        setTabTextColor(m_blinkTabs.at(i),palette().color(QPalette::WindowText));
    }
    m_blinkStatus = !m_blinkStatus;
    m_time->restart();
  }
}

void AdmTabBar::setTabNeedAttention(int tabIndex, bool needAttention)
{
  if(needAttention)
    addBlinkingTab(tabIndex);
  else
    delBlinkingTab(tabIndex);
}
