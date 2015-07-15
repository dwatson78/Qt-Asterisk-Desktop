#ifndef __ADMTABWIDGET_H__
#define __ADMTABWIDGET_H__

#include <QTabWidget>
#include "admtabbar.h"

namespace Ui
{
  class AdmTabWidget;
}

class AdmTabWidget : public QTabWidget
{
  Q_OBJECT
public:
  explicit AdmTabWidget(QWidget *parent = 0);

  void setTabNeedAttention(int tabIndex, bool bold);

public slots:

protected:

signals:

private:
  AdmTabBar * m_tabBar;
};

#endif // __ADMTABWIDGET_H__
