#ifndef ADMICONTEXT_H
#define ADMICONTEXT_H

#include <QFrame>
#include <QPixmap>
#include <QLabel>

namespace Ui {
  class AdmIconText;
}

class AdmIconText : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(QString  text      READ getText    WRITE setText)
  Q_PROPERTY(QPixmap  pixmap    READ getPixmap  WRITE setPixmap)
  
public:
  explicit AdmIconText(const QPixmap &icon, const QString &text, QWidget *parent = 0);
  explicit AdmIconText(QWidget *parent = 0);
  ~AdmIconText();

  const QString getText();
  void setText(const QString &text);

  const QPixmap getPixmap();
  void setPixmap(const QPixmap &pixmap);

  QLabel *getIconLabel();
  QLabel *getTextLabel();

protected:
  Ui::AdmIconText *ui;

signals:
  void destroying(AdmIconText*);

private:

};

#endif // ADMICONTEXT_H
