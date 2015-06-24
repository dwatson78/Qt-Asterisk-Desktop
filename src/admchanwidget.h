#ifndef ADMCHANWIDGET_H
#define ADMCHANWIDGET_H

#include <QFrame>
#include <QPixmap>

#include "astchannel.h"

namespace Ui {
class AdmChanWidget;
}

class AdmChanWidget : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(QString Text READ getText WRITE setText)
  Q_PROPERTY(QPixmap Pixmap READ getPixmap WRITE setPixmap)
  Q_PROPERTY(QString MoreText READ getMoreText WRITE setMoreText)
  Q_PROPERTY(QPixmap MorePixmap READ getMorePixmap WRITE setMorePixmap)
  
public:
  explicit AdmChanWidget(AstChannel *chan = 0, QFrame *parent = 0);
  ~AdmChanWidget();

  const QString getText();
  void setText(const QString &text);
  const QPixmap getPixmap();
  void setPixmap(const QPixmap &pixmap);

  const QString getMoreText();
  void setMoreText(const QString &text);
  const QPixmap getMorePixmap();
  void setMorePixmap(const QPixmap &pixmap);

public slots:
  void sUpdateChannel();
  void sShowHideMusic();

protected:

signals:
  void destroying(AdmChanWidget*);

private:
  Ui::AdmChanWidget *ui;
  AstChannel *_chan;
  bool _isMusicShowing;

};

#endif // ADMCHANWIDGET_H
