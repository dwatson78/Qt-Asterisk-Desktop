#ifndef __GLOOXMSGHANDLER_H__
#define __GLOOXMSGHANDLER_H__

#include <QObject>
#include <QSocketNotifier>
#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/connectiontcpclient.h>
#include <gloox/connectionlistener.h>
#include <gloox/rostermanager.h>
#include <gloox/rosterlistener.h>

using namespace std;
using namespace gloox;



class GlooxRosterListener : public QObject, public RosterManager, public RosterListener
{
  Q_OBJECT

public:
  explicit GlooxRosterListener(Client *client, QObject * parent = 0);
  
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

public slots:

protected:

signals:

private:
  Client  * _client;
  
};

class GlooxConnListener : public QObject, public ConnectionListener
{
  Q_OBJECT

public:
  explicit GlooxConnListener(Client *client, QObject * parent = 0);
  void onConnect();
  void onDisconnect(ConnectionError e);
  bool onTLSConnect(const CertInfo &info);

public slots:

protected:

signals:

private:
  Client              * _client;

};

class GlooxMsgHandler : public QObject, public MessageHandler
{
  Q_OBJECT

public:
  explicit GlooxMsgHandler(QObject *parent = 0);
  ~GlooxMsgHandler();
  
  void handleMessage(const Message &stanza, MessageSession *session = 0);
  
  
  
  static QString messageTypeToString(Message::MessageType type)
  {
    switch (type) {
        case Message::Chat:
            return "Chat";
        case Message::Error:
            return "Error";
        case Message::Groupchat:
            return "Groupchat";
        case Message::Headline:
            return "Headline";
        case Message::Normal:
            return "Normal";
        case Message::Invalid:
            return "Invalid";
        default:
            return "unknown type";
    }
  }
  static QString stanzaToString(const Message& stanza)
  {
    return QString("type:'%1' from:'%2' body: '%3'")
              .arg(GlooxMsgHandler::messageTypeToString(stanza.subtype()))
              .arg(QString::fromStdString(stanza.from().full()))
              .arg(QString::fromStdString(stanza.body()))
    ;
  }


public slots:
  void sSockectActivated(int socket);
  
protected:

signals:

private:
  Client              * _client;
  GlooxConnListener   * _connListener;
  ConnectionTCPClient * _conn;
  GlooxRosterListener  * _rosterMgr;
  QSocketNotifier     * _notif;
  
  int eventCounter;
};

#endif // __GLOOXMSGHANDLER_H__
