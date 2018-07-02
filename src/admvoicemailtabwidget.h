#ifndef ADMVOICEMAILTABWIDGET_H
#define ADMVOICEMAILTABWIDGET_H

#include <QFrame>

#include "admvoicemailwidget.h"

namespace Ui {
class AdmVoiceMailTabWidget;
}

class AdmVoiceMailTabWidget : public QFrame
{
  Q_OBJECT
  
public:
  explicit AdmVoiceMailTabWidget(QWidget *parent = 0);
  ~AdmVoiceMailTabWidget();

  void addVoiceMailWidget(AdmVoiceMailWidget *widget);
  void clearVoiceMailWidgets();
  
private:
  Ui::AdmVoiceMailTabWidget *ui;
};

#endif // ADMVOICEMAILTABWIDGET_H
