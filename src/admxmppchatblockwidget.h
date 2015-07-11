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
public slots :
  void sDocumentSizeChanged(const QSizeF &size);
signals:
  void destroying(AdmXmppChatBlockWidget *);

private:
  Ui::AdmXmppChatBlockWidget *ui;
  QString _msgBody;
  void resizeChatText();
  //QString& doWrap(const QString &text);
};

#endif // ADMXMPPCHATBLOCKWIDGET_H

