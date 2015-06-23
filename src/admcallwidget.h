#ifndef ADMCALLWIDGET_H
#define ADMCALLWIDGET_H

#include <QFrame>

namespace Ui {
  class AdmCallWidget;
}

class AdmCallWidget : public QFrame
{
  Q_OBJECT

public:
  explicit AdmCallWidget(QFrame *parent = 0);
  ~AdmCallWidget();

public slots:

protected:

signals:

private:
  Ui::AdmCallWidget *ui;
};

#endif // ADMCALLWIDGET_H
