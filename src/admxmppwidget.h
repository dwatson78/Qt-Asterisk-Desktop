#ifndef ADMXMPPWIDGET_H
#define ADMXMPPWIDGET_H

#include <QFrame>
#include <QSocketNotifier>
#include <QModelIndex>
#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/messageeventhandler.h>
#include <gloox/messagesession.h>
#include <gloox/messagesessionhandler.h>
#include <gloox/chatstatehandler.h>
#include <gloox/connectiontcpclient.h>
#include <gloox/connectionlistener.h>
#include <gloox/rostermanager.h>
#include <gloox/rosterlistener.h>

#include "admxmppbuddywidget.h"
#include "admxmppchatwidget.h"

namespace Ui {
  class AdmXmppWidget;
}

using namespace gloox;

class AdmXmppWidget : public QFrame
                      , public ConnectionListener
                      , public RosterListener
                      , public MessageSessionHandler
                      //, public MessageHandler
                      //, public MessageEventHandler
                      //, public ChatStateHandler
{
  Q_OBJECT
  
public:
  explicit AdmXmppWidget(QWidget *parent = 0);
  ~AdmXmppWidget();
  
  /* Reimplemented methods for ConnectionListener */
  void onConnect();
  void onDisconnect(ConnectionError e);
  bool onTLSConnect(const CertInfo &info);

  /* Reimplemented methods for RosterListener */
  void 	handleItemAdded (const JID &jid);
  void 	handleItemSubscribed (const JID &jid);
  void 	handleItemRemoved (const JID &jid);
  void 	handleItemUpdated (const JID &jid);
  void 	handleItemUnsubscribed (const JID &jid);
  void 	handleRoster (const Roster &roster);
  void 	handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg);
  void 	handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg);
  bool 	handleSubscriptionRequest (const JID &jid, const std::string &msg);
  bool 	handleUnsubscriptionRequest (const JID &jid, const std::string &msg);
  void 	handleNonrosterPresence (const Presence &presence);
  void 	handleRosterError (const IQ &iq);

  /* Reimplemented methods for MessageSessionHandler */
  void handleMessageSession(MessageSession *session);

  /* Reimplemented methods for MessageHandler */
  //void handleMessage(const Message &msg, MessageSession *session);

  /* Reimplemented methods for MessageEventHandler */
  //void handleMessageEvent(const JID &from, MessageEventType event);

  /* Reimplemented methods for ChatStateHandler */
  //void handleChatState(const JID &from, ChatStateType state);

public slots:
  void sSockectActivated(int fd);
  void sStatusIndexChanged(int index);

  void sDestroyingChatWidget(AdmXmppChatWidget* obj);

  void sBuddyActivated(const QModelIndex &index);
  void sDestroyingBuddy(AdmXmppBuddyWidget *buddy);
private:
  Ui::AdmXmppWidget *ui;

  Client              * _client;
  ConnectionTCPClient * _conn;
  QSocketNotifier     * _socketNotify;
  QMap<JID,AdmXmppBuddyWidget*>     * _mapBuddies;
  QMap<QString,MessageSession*>     * _mapSessions;
  QMap<QString,AdmXmppChatWidget*>  * _mapChats;
  void _connect();
};

#endif // ADMXMPPWIDGET_H
