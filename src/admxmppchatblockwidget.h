#ifndef ADMXMPPCHATBLOCKWIDGET_H
#define ADMXMPPCHATBLOCKWIDGET_H

#include <QFrame>
#include <QDateTime>

namespace Ui {
class AdmXmppChatBlockWidget;
}

class AdmXmppChatBlockWidget : public QFrame
{
  Q_OBJECT
  
public:
  explicit AdmXmppChatBlockWidget(QWidget *parent = 0);
  ~AdmXmppChatBlockWidget();
  
  void setName(const QString &name);
  void setTime(const QDateTime &time);
  void setText(const QString &text);
  void appendText(const QString &text);
  
signals:
  void destroying(AdmXmppChatBlockWidget *);

private:
  Ui::AdmXmppChatBlockWidget *ui;
};

#endif // ADMXMPPCHATBLOCKWIDGET_H

