#ifndef __ADMTABBAR_H__
#define __ADMTABBAR_H__

#include <QTabBar>
#include <QList>
#include <QTime>
#include <QTimer>

namespace Ui
{
  class AdmTabWidget;
}

class AdmTabBar : public QTabBar
{
  Q_OBJECT
public:
  explicit AdmTabBar(QWidget *parent = 0);
  
    void setTabNeedAttention(int tabIndex, bool needAttention);

public slots:
  void sTickTock();

protected:

signals:

private:
  QTime      * m_time;
  QTimer     * m_timer;
  QList<int>   m_blinkTabs;
  bool         m_blinkStatus;
  void addBlinkingTab(int tabIndex);
  void delBlinkingTab(int tabIndex);
};

#endif // __ADMTABWIDGET_H__
