#ifndef ADMEXTENSIONWIDGET_H
#define ADMEXTENSIONWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QVariantMap>

#include "astsippeer.h"

namespace Ui {
class AdmExtensionWidget;
}

class AdmExtensionWidget : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(QString  Exten   READ getExten   WRITE setExten)
  Q_PROPERTY(QString  Text    READ getText    WRITE setText)
  Q_PROPERTY(QPixmap  Pixmap  READ getPixmap  WRITE setPixmap)
  
public:
  explicit AdmExtensionWidget(QWidget *parent = 0);
  ~AdmExtensionWidget();

  const QString getExten();
  void setExten(const QString &exten);
  void setExten(const QVariant &exten);

  const QString getText();
  void setText(const QString &text);

  const QPixmap getPixmap();
  void setPixmap(const QPixmap &pixmap);

  QLabel *getIconLabel();
  QLabel *getExtenLabel();
  QLabel *getTextLabel();

  void setSipPeer(AstSipPeer *peer);
  AstSipPeer *getSipPeer();

public slots:
  void sSipPeerDestroying(AstSipPeer *peer);
  void sExtensionStatusEvent(const QVariantMap &event);
  void sExtensionDndStatusEvent(const QVariantMap &event);
  void sSipPeerUpdated(AstSipPeer *peer);
  void sSipExtensionStatusEvent(AstSipPeer *peer, const QVariantMap &event);
  void sSipExtensionDndStatusEvent(AstSipPeer *peer, const QVariantMap &event, bool isDndOn);

protected:
  void dragEnterEvent (QDragEnterEvent  *event);
  void dragMoveEvent  (QDragMoveEvent   *event);
  void dragLeaveEvent (QDragLeaveEvent  *event);
  void dropEvent      (QDropEvent       *event);
signals:
  void destroying(AdmExtensionWidget *);
  void sigDragEnterEvent(AdmExtensionWidget *obj, QDragEnterEvent  *event);
  void sigDragMoveEvent (AdmExtensionWidget *obj, QDragMoveEvent   *event);
  void sigDragLeaveEvent(AdmExtensionWidget *obj, QDragLeaveEvent  *event);

private:
  Ui::AdmExtensionWidget  * ui;
  AstSipPeer              * _sipPeer;
  uint                      _statusNum;
  bool                      _isDndOn;

  void _updateStatusIcon();
  void _updateStatusFromSipPeerStatus(const QString &status);
};

#endif // ADMEXTENSIONWIDGET_H
