#ifndef ADMINTERCOMVIDEODIALOG_H
#define ADMINTERCOMVIDEODIALOG_H

#include <QDialog>

namespace Ui {
class AdmIntercomVideoDialog;
}

class AdmIntercomVideoDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit AdmIntercomVideoDialog(QWidget *parent = 0);
  ~AdmIntercomVideoDialog();
  
private:
  Ui::AdmIntercomVideoDialog *ui;
};

#endif // ADMINTERCOMVIDEODIALOG_H
