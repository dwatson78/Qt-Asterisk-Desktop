#ifndef ADMINTERCOMVIDEODIALOG_H
#define ADMINTERCOMVIDEODIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>

namespace Ui {
class AdmIntercomVideoDialog;
}

class AdmIntercomVideoDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit AdmIntercomVideoDialog(QWidget *parent = 0, QUrl url = QUrl());
  ~AdmIntercomVideoDialog();
  
public slots:
  void SBtnClicked(QAbstractButton *btn);
  void SRefreshVideo();
  void SOpenDoor();
  void SStart();
  void SStop();

private:
  Ui::AdmIntercomVideoDialog *ui;
  QUrl _url;
  Phonon::MediaObject *_mObj;
  Phonon::MediaSource _mSrc;

};

#endif // ADMINTERCOMVIDEODIALOG_H
