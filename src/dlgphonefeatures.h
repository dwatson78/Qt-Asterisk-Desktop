#ifndef DLGPHONEFEATURES_H
#define DLGPHONEFEATURES_H

#include <QDialog>
#include <QShowEvent>

namespace Ui {
class DlgPhoneFeatures;
}

class DlgPhoneFeatures : public QDialog
{
  Q_OBJECT
  
public:
  explicit DlgPhoneFeatures(QWidget *parent = 0);
  ~DlgPhoneFeatures();

protected:
  void showEvent(QShowEvent *);

private:
  Ui::DlgPhoneFeatures *ui;
};

#endif // DLGPHONEFEATURES_H
