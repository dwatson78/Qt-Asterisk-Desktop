#ifndef ADMXMPPCHATWIDGET_H
#define ADMXMPPCHATWIDGET_H

#include <QFrame>
#include <QPlainTextEdit>
#include <QListWidgetItem>
#include <gloox/gloox.h>
#include <gloox/jid.h>
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

class AdmXmppChatWidget : 	public QFrame
                          , public MessageHandler
                          , public MessageEventHandler
                          , public ChatStateHandler
{
  
  Q_OBJECT
  
public:
  explicit AdmXmppChatWidget(MessageSession *session = 0, QWidget *parent = 0);
  ~AdmXmppChatWidget();

  void setSelfJid(JID jid);

  MessageSession *messageSession();
  void setMessageSession(MessageSession *session);

  void handleMessage(const Message &msg, MessageSession *session);
  void handleMessageEvent(const JID &from, MessageEventType event);
  void handleChatState(const JID &from, ChatStateType state);
  
  static void setHeight (QPlainTextEdit *edit, int nRows);
  
  void focusChatText();

public slots:
  void sNewChatBlockCountChanged(int);
  void sNewChatContentsChanged();
  void sTickTock();

protected:
  bool eventFilter(QObject *obj, QEvent *event);

signals:
  void destroying(AdmXmppChatWidget *obj);
  void attention(AdmXmppChatWidget *obj);

private:
  Ui::AdmXmppChatWidget *ui;
  MessageSession      * _session;
  MessageEventFilter  * _messageEventFilter;
  ChatStateFilter     * _chatStateFilter;
  QListWidgetItem     * _lastChatBlock;
  JID                   _selfJid;
  JID                   _lastJid;
  QTime               * _time;
  ChatStateType         _lastChatState;

  void addMsg(const JID &jid, const Message &msg);
  void addChatBlock(const JID &jid, const Message &msg);
  
  void setChatStateComposing(bool composing, bool afterSending = false);
  void setChatState(ChatStateType chatState);
};

#endif // ADMXMPPCHATWIDGET_H
