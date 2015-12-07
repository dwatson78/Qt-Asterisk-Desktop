#ifndef ADMINTERCOMVIDEODIALOG_H
#define ADMINTERCOMVIDEODIALOG_H

#include <QDialog>
#include <QUrl>
#include <QAbstractButton>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>

#include "astchannel.h"

namespace Ui {
class AdmIntercomVideoDialog;
}

class AdmIntercomVideoDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit AdmIntercomVideoDialog(QWidget *parent = 0, AstChannel *chan = 0, QUrl url = QUrl(), QString dtmfSequence = QString());
  ~AdmIntercomVideoDialog();
  
public slots:
  void SBtnClicked(QAbstractButton *btn);
  void SRefreshVideo();
  void SOpenDoor();
  void sEnableOpenDoor();
  void SStart();
  void SStop();

  void sHangupChannel(AstChannel* channel);
  void sRemoveChannel(AstChannel* channel);


private:
  Ui::AdmIntercomVideoDialog  *ui;
  AstChannel                  *_chan;
  QUrl                        _url;
  QString                     _dtmfSequence;
  Phonon::MediaObject         *_mObj;
  Phonon::MediaSource         _mSrc;
};

#endif // ADMINTERCOMVIDEODIALOG_H
