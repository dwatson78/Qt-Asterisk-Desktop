#ifndef ADMXMPPCHATWIDGET_H
#define ADMXMPPCHATWIDGET_H

#include <QFrame>
#include <QPlainTextEdit>
#include <gloox/gloox.h>
#include <gloox/messagehandler.h>
#include <gloox/message.h>
#include <gloox/messagesession.h>
#include <gloox/messageeventfilter.h>
#include <gloox/messageeventhandler.h>
#include <gloox/chatstatefilter.h>
#include <gloox/chatstatehandler.h>

namespace Ui {
class AdmXmppChatWidget;
}

using namespace gloox;

class AdmXmppChatWidget : public QFrame
                          , public MessageHandler
                          //, public MessageFilter
                          , public MessageEventHandler
                          , public ChatStateHandler
{
  Q_OBJECT
  
public:
  explicit AdmXmppChatWidget(MessageSession *session = 0, QWidget *parent = 0);
  ~AdmXmppChatWidget();

  MessageSession *messageSession();
  void setMessageSession(MessageSession *session);

  void handleMessage(const Message &msg, MessageSession *session);
  void handleMessageEvent(const JID &from, MessageEventType event);
  void handleChatState(const JID &from, ChatStateType state);
  //void attachTo(MessageSession *session);
  //void decorate(Message &msg);
  //void filter(Message &msg);

  static void setHeight (QPlainTextEdit *edit, int nRows);

public slots:
  void sNewChatBlockCountChanged(int);
protected:

  bool eventFilter(QObject *obj, QEvent *event);
signals:
  void destroying(AdmXmppChatWidget *obj);
private:
  Ui::AdmXmppChatWidget *ui;
  MessageSession      * _session;
  MessageEventFilter  * _messageEventFilter;
  ChatStateFilter     * _chatStateFilter;
};

#endif // ADMXMPPCHATWIDGET_H