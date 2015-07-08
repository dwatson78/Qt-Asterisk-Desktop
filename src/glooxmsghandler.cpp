#include "glooxmsghandler.h"

#include <QDebug>
#include <QSettings>

#include "qtasteriskdesktopprefs.h"

GlooxConnListener::GlooxConnListener(Client *client, QObject * parent) :
  QObject(parent),
  ConnectionListener()
{
  _client = client;
}

void GlooxConnListener::onConnect()
{
  qDebug() << "ConnListener::onConnect()" << endl;
}
void GlooxConnListener::onDisconnect(ConnectionError e)
{
  Q_UNUSED(e)
  qDebug() << "ConnListener::onDisconnect() " << e << endl;
}
bool GlooxConnListener::onTLSConnect(const CertInfo& info)
{
  Q_UNUSED(info)
  qDebug() << "ConnListener::onTLSConnect()" << endl;

  return true;
}

/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/

GlooxRosterListener::GlooxRosterListener(Client *client, QObject * parent) :
  QObject(parent),
  RosterManager(client),
  RosterListener()
{
  _client = client;
  _client->rosterManager()->registerRosterListener(this, false); //Register as an asynchronous listener
}

void GlooxRosterListener::handleItemAdded (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemAdded: %1")
              .arg(QString::fromStdString(jid.full()));
}
void GlooxRosterListener::handleItemSubscribed (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemSubscribed: %1")
              .arg(QString::fromStdString(jid.full()));
}
void GlooxRosterListener::handleItemRemoved (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemRemoved: %1")
              .arg(QString::fromStdString(jid.full()));
}
void GlooxRosterListener::handleItemUpdated (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemUpdated: %1")
              .arg(QString::fromStdString(jid.full()));
}
void GlooxRosterListener::handleItemUnsubscribed (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemUnsubscribed: %1")
              .arg(QString::fromStdString(jid.full()));
}
void GlooxRosterListener::handleRoster (const Roster &roster)
{
  Q_UNUSED(roster)
  qDebug() << QString("GlooxRosterListener::handleRoster");
  
  std::map<const std::string, RosterItem*>::const_iterator it;
  for(it = roster.begin(); it != roster.end(); ++it)
  {
    qDebug() << QString::fromStdString(it->first);
    qDebug() << QString("Online? %1").arg(it->second->online());
  }
}
void GlooxRosterListener::handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg)
{
  Q_UNUSED(item)
  Q_UNUSED(resource)
  Q_UNUSED(presence)
  Q_UNUSED(msg)
  qDebug() <<   QString("GlooxRosterListener::handleRosterPresence: %1 %2 %4 (%5)")
                .arg(QString::fromStdString(resource))
                .arg(QString::fromStdString(item.jidJID().username()))
                .arg(presence)
                .arg(QString::fromStdString(msg));
}
void GlooxRosterListener::handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg)
{
  Q_UNUSED(item)
  Q_UNUSED(resource)
  Q_UNUSED(presence)
  Q_UNUSED(msg)
  qDebug() << QString("GlooxRosterListener::handleSelfPresence: %1")
              .arg(QString::fromStdString(msg));
}
bool GlooxRosterListener::handleSubscriptionRequest (const JID &jid, const std::string &msg)
{
  Q_UNUSED(jid)
  Q_UNUSED(msg)

  qDebug() << "handleSubscriptionRequest";

  // Automatically allow for now
  // TODO: Make this a preference variable
  // TODO: Get the preference value and if false, then present to the
  // TODO: user the opportunity to allow or block the subscription
  // TODO: request.
  ackSubscriptionRequest(jid, true);
  
  // Now subscribe to the user that subscribed to me
  subscribe(jid, jid.username(), StringList(), "Auto Subscribe");
  
  return true; // This is ignored in asynchronous mode
}
bool GlooxRosterListener::handleUnsubscriptionRequest (const JID &jid, const std::string &msg)
{
  Q_UNUSED(jid)
  Q_UNUSED(msg)

  qDebug() << "handleUnsubscriptionRequest";

  // Unsubsribe asynchronously
  unsubscribe(jid, msg); 
  return true; // This is ignored in asynchronous mode
}
void GlooxRosterListener::handleNonrosterPresence (const Presence &presence)
{
  Q_UNUSED(presence)
  qDebug() << QString("GlooxRosterListener::handleNonrosterPresence : %1:  %2 (%3)")
              .arg(QString::fromStdString(presence.from().username()))
              .arg(presence.presence())
              .arg(QString::fromStdString(presence.status()));
}
void GlooxRosterListener::handleRosterError (const IQ &iq)
{
  Q_UNUSED(iq)
  qDebug() << QString("GlooxRosterListener::handleRosterError : %1").arg(iq.subtype());
}

/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/

GlooxMsgHandler::GlooxMsgHandler(QObject * parent) :
  QObject(parent),
  MessageHandler()
{
  QSettings set;
  set.beginGroup("XMPP");
  if(     set.contains(XmppPref::getName(XmppPref::host))
      &&  set.contains(XmppPref::getName(XmppPref::userName))
      &&  set.contains(XmppPref::getName(XmppPref::realm))
      &&  set.contains(XmppPref::getName(XmppPref::secPass)))
  {
    QString username = QString("%1@%2")
                       .arg(set.value(XmppPref::getName(XmppPref::userName)).toString())
                       .arg(set.value(XmppPref::getName(XmppPref::realm)).toString());
    QString password = set.value(XmppPref::getName(XmppPref::secPass)).toString();
    
    QString host = set.value(XmppPref::getName(XmppPref::host)).toString();
    int port = -1;
    if(set.contains(XmppPref::getName(XmppPref::host)))
      port = set.value(XmppPref::getName(XmppPref::port)).toInt();
    if(port == 0)
      port = -1;

    qDebug() << username << password << host << port;
    JID jid(username.toUtf8().constData());
    _client = new Client(jid,password.toUtf8().constData());
    _client->registerMessageHandler(this);

    _rosterMgr = new GlooxRosterListener(_client, this);

    _connListener = new GlooxConnListener(_client, this);
    _client->registerConnectionListener(_connListener);
    
    _conn = new ConnectionTCPClient( _client, _client->logInstance(), host.toUtf8().constData(), port);
    _client->setConnectionImpl(_conn);
    
    _client->connect(false);
    _notif = new QSocketNotifier(_conn->socket(), QSocketNotifier::Read, this);
    connect(_notif, SIGNAL(activated(int)),
            this,   SLOT(sSockectActivated(int)));
  }
  
}

GlooxMsgHandler::~GlooxMsgHandler()
{
  delete _client;
  delete _conn;
  delete _connListener;
}

void GlooxMsgHandler::handleMessage(const Message &stanza, MessageSession *session)
{
  Q_UNUSED(session)

  qDebug() << "Received message: " << GlooxMsgHandler::stanzaToString(stanza);
  Message msg(Message::Chat, stanza.from(), "Tell me more about " + stanza.body() );
  _client->send( msg );
}

void GlooxMsgHandler::sSockectActivated(int socket)
{
  eventCounter += 1;
  Q_UNUSED(socket);
  qDebug() << "GlooxMsgHandler::sSockectActivated: " << eventCounter;
  ConnectionError e = _client->recv();
  qDebug() << "Connection Error: " << e << eventCounter;
}
