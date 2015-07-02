#ifndef ADMVOICEMAILWIDGET_H
#define ADMVOICEMAILWIDGET_H

#include <QWidget>
#include <QVariantMap>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QListWidgetItem>
#include <QTableWidgetItem>

namespace Ui {
class AdmVoiceMailWidget;
}

class AdmVoiceMailWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit AdmVoiceMailWidget(QString vmBox, QWidget *parent = 0);
  ~AdmVoiceMailWidget();

  const QString &getVmBox(){return _vmBox;}

public slots:
  void sVmCountReady(const QVariantMap &data);
  void sVmCountError(QNetworkReply::NetworkError err);

  void sVmMsgDetailsReady(const QVariantMap &data);
  void sVmMsgDetailsError(QNetworkReply::NetworkError err);

  void sFolderItemActivated(QListWidgetItem *current, QListWidgetItem * previous);

  void sMessagesItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

  void sPlayMsgClicked();
  void sSoundFileReady(const QByteArray &data);

private:
  Ui::AdmVoiceMailWidget *ui;
  QString _vmBox;
};

#endif // ADMVOICEMAILWIDGET_H
