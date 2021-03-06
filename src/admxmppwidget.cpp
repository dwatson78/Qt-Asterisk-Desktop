#include "admxmppwidget.h"
#include "ui_admxmppwidget.h"

#include <QMessageBox>
#include <QSettings>
#include <QListWidgetItem>
#include <QDebug>

#include "qtasteriskdesktopprefs.h"

#include <gloox/presence.h>
#include <gloox/disco.h>

#include <gloox/attention.h>
#include <gloox/chatstate.h>

AdmXmppWidget::AdmXmppWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmXmppWidget)
{
  ui->setupUi(this);
  _client = NULL;
  _conn = NULL;
  _socketNotify = NULL;

  _mapBuddies = new QMap<JID,AdmXmppBuddyWidget*>();
  _mapBuddyItems = new QMap<JID,AdmXmppBuddyItem*>();
  _mapChats = new QMap<QString,AdmXmppChatWidget*>();
  _mapSessions = new QMap<QString,MessageSession*>();
  
  connect(ui->_status,  SIGNAL(currentIndexChanged(int)),
          this,         SLOT(sStatusIndexChanged(int))
  );

  connect(ui->_buddyList, SIGNAL(activated(QModelIndex)),
          this,           SLOT(sBuddyActivated(QModelIndex)));

  connect(ui->_conversations, SIGNAL(currentChanged(int)),
          this,               SLOT(sConversationsCurrentChanged(int)));

  connect(ui->_conversations, SIGNAL(tabCloseRequested(int)),
          this,               SLOT(sConversationsTabClosed(int)));
          
  // Customize colors
  ui->_buddyList->setStyleSheet("QListWidget::item:selected {background: palette(button);}");
  ui->_buddyList->setSortingEnabled(true);

  // Connect to XMPP
  _connect();
}

AdmXmppWidget::~AdmXmppWidget()
{
  //Remove the conversations so they can set their chat state as gone
  delete ui->_conversations;

  // delete the user interface
  delete ui;
  
  // delete the XMPP client
  delete _client;
  delete _socketNotify;
  _conn = NULL;

  // delete local maps
  delete _mapSessions;
  delete _mapChats;
  delete _mapBuddyItems;
  delete _mapBuddies;
}

void AdmXmppWidget::onConnect()
{
  ui->_status->blockSignals(true);
  ui->_status->setCurrentIndex(0);
  ui->_status->blockSignals(false);
}

void AdmXmppWidget::onDisconnect(ConnectionError e)
{
  ui->_status->blockSignals(true);
  ui->_status->setCurrentIndex(4);
  ui->_status->blockSignals(false);
  if(e == ConnNoError)
    return; /* Disconnection was expected... */
  QString errMsg;
  switch(e)
  {
    case ConnStreamError:
      errMsg = "Connection stream error: ";
      switch(_client->streamError())
      {
        case StreamErrorBadFormat:
          errMsg = errMsg.append("bad format");
          break;
        case StreamErrorBadNamespacePrefix:
          errMsg = errMsg.append("bad XML namespace prefix");
          break;
        case StreamErrorConflict:
          errMsg = errMsg.append("new connection stream conflicts with this one");
          break;
        case StreamErrorConnectionTimeout:
          errMsg = errMsg.append("timed out");
          break;
        case StreamErrorHostGone:
          errMsg = errMsg.append("host is gone");
          break;
        case StreamErrorHostUnknown:
          errMsg = errMsg.append("host is unknown");
          break;
        case StreamErrorImproperAddressing:
          errMsg = errMsg.append("improper from or to address");
          break;
        case StreamErrorInternalServerError:
          errMsg = errMsg.append("internal server error");
          break;
        case StreamErrorInvalidFrom:
          errMsg = errMsg.append("not authenticated or allowed");
          break;
        case StreamErrorInvalidId:
          errMsg = errMsg.append("stream id is invalid");
          break;
        case StreamErrorInvalidNamespace:
          errMsg = errMsg.append("XML namespace is invalid");
          break;
        case StreamErrorInvalidXml:
          errMsg = errMsg.append("XML is invalid");
          break;
        case StreamErrorNotAuthorized:
          errMsg = errMsg.append("not authorized");
          break;
        case StreamErrorPolicyViolation:
          errMsg = errMsg.append("access police has been violated");
          break;
        case StreamErrorRemoteConnectionFailed:
          errMsg = errMsg.append("remote connection failed");
          break;
        case StreamErrorResourceConstraint:
          errMsg = errMsg.append("server lacks the resourcses to service this stream");
          break;
        case StreamErrorRestrictedXml:
          errMsg = errMsg.append("attempted to use a restricted XML feature");
          break;
        case StreamErrorSeeOtherHost:
          errMsg = errMsg.append("connect to the alternative host");
          break;
        case StreamErrorSystemShutdown:
          errMsg = errMsg.append("server is shutting down");
          break;
        case StreamErrorUnsupportedEncoding:
          errMsg = errMsg.append("unsupported encoding");
          break;
        case StreamErrorUnsupportedStanzaType:
          errMsg = errMsg.append("stanza type not supported");
          break;
        case StreamErrorUnsupportedVersion:
          errMsg = errMsg.append("unsupported version");
          break;
        case StreamErrorXmlNotWellFormed:
          errMsg = errMsg.append("XML could not be parsed");
          break;
        case StreamErrorUndefined:
        default:
          errMsg = errMsg.append("Unkown");
          break;
      }
      if(!_client->streamErrorText().empty())
        errMsg.append(QString("\nText: %1").arg(QString::fromUtf8(_client->streamErrorText().data())));
      if(!_client->streamErrorCData().empty())
        errMsg.append(QString("\nCData: %1").arg(QString::fromUtf8(_client->streamErrorCData().data())));
      break;
    case ConnStreamVersionError:         /**< The incoming stream's version is not supported */
      errMsg = "Incoming stream version not supported";
      break;
    case ConnStreamClosed:               /**< The stream has been closed (by the server). */
      errMsg = "Connection closed by server";
      break;
    case ConnProxyAuthRequired:          /**< The HTTP/SOCKS5 proxy requires authentication.
                                       * @since 0.9 */
      errMsg = "Connection proxy authentication is required";
      break;
    case ConnProxyAuthFailed:            /**< HTTP/SOCKS5 proxy authentication failed.
                                       * @since 0.9 */
      errMsg = "Connection proxy authentication failed";
      break;
    case ConnProxyNoSupportedAuth:       /**< The HTTP/SOCKS5 proxy requires an unsupported auth mechanism.
                                       * @since 0.9 */
      errMsg = "Connection proxy requires an unsupported authentication mechanism";
      break;
    case ConnIoError:                    /**< An I/O error occured. */
      errMsg = "An I/O error occured";
      break;
    case ConnParseError:                 /**< An XML parse error occurred. */
      errMsg = "An XML parse error occurred";
      break;
    case ConnConnectionRefused:          /**< The connection was refused by the server (on the socket level).
                                       * @since 0.9 */
      errMsg = "Connection refused";
      break;
    case ConnDnsError:                   /**< Resolving the server's hostname failed.
                                       * @since 0.9 */
      errMsg = "Could not resolve host name";
      break;
    case ConnOutOfMemory:                /**< Out of memory. Uhoh. */
      errMsg = "Connection out of memory";
      break;
    case ConnNoSupportedAuth:            /**< The auth mechanisms the server offers are not supported
                                       * or the server offered no auth mechanisms at all. */
      errMsg = "The authentication methods offered by the host are not supported";
      break;
    case ConnTlsFailed:                  /**< The server's certificate could not be verified or the TLS
                                       * handshake did not complete successfully. */
      errMsg = "Could not establish a TLS connection";
      break;
    case ConnTlsNotAvailable:            /**< The server didn't offer TLS while it was set to be required,
                                       * or TLS was not compiled in.
                                       * @since 0.9.4 */
      errMsg = "TLS required but the hos t did not offer any";
      break;
    case ConnCompressionFailed:          /**< Negotiating/initializing compression failed.
                                       * @since 0.9 */
      errMsg = "Connection compression could not be established";
      break;
    case ConnAuthenticationFailed:       /**< Authentication failed. Username/password wrong or account does
                                       * not exist. Use ClientBase::authError() to find the reason. */
      errMsg = "Authentication failed";
      if(_client->authError() != AuthErrorUndefined)
      {
        errMsg = errMsg.append(": ");
        switch(_client->authError())
        {
          case SaslAborted:                    /**< The receiving entity acknowledges an &lt;abort/&gt; element sent
                                           * by the initiating entity; sent in reply to the &lt;abort/&gt;
                                           * element. */
            errMsg = errMsg.append("aborted");
            break;
          case SaslIncorrectEncoding:          /**< The data provided by the initiating entity could not be processed
                                           * because the [BASE64] encoding is incorrect (e.g., because the encoding
                                           * does not adhere to the definition in Section 3 of [BASE64]); sent in
                                           * reply to a &lt;response/&gt; element or an &lt;auth/&gt; element with
                                           * initial response data. */
            errMsg = errMsg.append("incorrect encoding");
            break;
          case SaslInvalidAuthzid:             /**< The authzid provided by the initiating entity is invalid, either
                                           * because it is incorrectly formatted or because the initiating entity
                                           * does not have permissions to authorize that ID; sent in reply to a
                                           * &lt;response/&gt; element or an &lt;auth/&gt; element with initial
                                           * response data.*/
            errMsg = errMsg.append("the authzid provided is invalid");
            break;
          case SaslInvalidMechanism:           /**< The initiating entity did not provide a mechanism or requested a
                                           * mechanism that is not supported by the receiving entity; sent in reply
                                           * to an &lt;auth/&gt; element. */
            errMsg = errMsg.append("invalid authentication mechanism");
            break;
          case SaslMalformedRequest:           /**< The request is malformed (e.g., the &lt;auth/&gt; element includes
                                           * an initial response but the mechanism does not allow that); sent in
                                           * reply to an &lt;abort/&gt;, &lt;auth/&gt;, &lt;challenge/&gt;, or
                                           * &lt;response/&gt; element. */
            errMsg = errMsg.append("malformed request");
            break;
          case SaslMechanismTooWeak:           /**< The mechanism requested by the initiating entity is weaker than
                                           * server policy permits for that initiating entity; sent in reply to a
                                           * &lt;response/&gt; element or an &lt;auth/&gt; element with initial
                                           * response data. */
            errMsg = errMsg.append("authentication mechanism is too weak");
            break;
          case SaslNotAuthorized:              /**< The authentication failed because the initiating entity did not
                                           * provide valid credentials (this includes but is not limited to the
                                           * case of an unknown username); sent in reply to a &lt;response/&gt;
                                           * element or an &lt;auth/&gt; element with initial response data. */
          case NonSaslNotAuthorized:            /**< @xep{0078}: Incorrect Credentials */
            errMsg = errMsg.append("invalid user name or password");
            break;
          case SaslTemporaryAuthFailure:       /**< The authentication failed because of a temporary error condition
                                           * within the receiving entity; sent in reply to an &lt;auth/&gt; element
                                           * or &lt;response/&gt; element. */
            errMsg = errMsg.append("temporary authentication failure");
            break;
          case NonSaslConflict:                /**< @xep{0078}: Resource Conflict */
            errMsg = errMsg.append("resource conflict");
            break;
          case NonSaslNotAcceptable:           /**< @xep{0078}: Required Information Not Provided */
            errMsg = errMsg.append("required information not provided");
            break;
          default:
            break;
        }
      }
      break;
    case ConnUserDisconnected:           /**< The user (or higher-level protocol) requested a disconnect. */
      errMsg = "You caused this!"; //TODO: remove this!
      break;
    case ConnNotConnected:
      errMsg = "Not connected";
      break;
    default:
      break;
  }
  if(!errMsg.isNull() && !errMsg.isEmpty())
  {
    QMessageBox(QMessageBox::Critical,"Disconnected!",errMsg);
  }
}

bool AdmXmppWidget::onTLSConnect(const CertInfo &info)
{
  Q_UNUSED(info)

  return true;
}

void AdmXmppWidget::handleItemAdded (const JID &jid)
{
  _addBuddyIfNotExist(jid);
}
void AdmXmppWidget::handleItemSubscribed (const JID &jid)
{
  _addBuddyIfNotExist(jid);
}
void AdmXmppWidget::handleItemRemoved (const JID &jid)
{
  if(_mapBuddies->contains(jid))
    _mapBuddies->value(jid)->deleteLater();
}
void AdmXmppWidget::handleItemUpdated (const JID &jid)
{
  QString name = QString::fromUtf8(_client->rosterManager()->getRosterItem(jid)->name().data());
  if(_mapBuddies->contains(jid))
    _mapBuddies->value(jid)->setName(name);
}
void AdmXmppWidget::handleItemUnsubscribed (const JID &jid)
{
  if(_mapBuddies->contains(jid))
    _mapBuddies->value(jid)->deleteLater();
}
void AdmXmppWidget::handleRoster (const Roster &roster)
{
  std::map<const std::string, RosterItem*>::const_iterator it;
  for(it = roster.begin(); it != roster.end(); ++it)
  {
    _addBuddyIfNotExist(it->second->jidJID(), false);
  }
  ui->_buddyList->sortItems();
}
void AdmXmppWidget::handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg)
{
  AdmXmppBuddyWidget *buddy = _addBuddyIfNotExist(item.jidJID());
  if(NULL != buddy)
  {
    buddy->setPresence(item, QString::fromUtf8(resource.data()), presence, QString::fromUtf8(msg.data()));
  }
}
void AdmXmppWidget::handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg)
{
  Q_UNUSED(item)
  Q_UNUSED(resource)
  Q_UNUSED(msg)

  ui->_status->blockSignals(true);
  switch(presence)
  {
    case Presence::Available:
      ui->_status->setCurrentIndex(0);
      break;
    case Presence::Chat:
      ui->_status->setCurrentIndex(1);
      break;
    case Presence::Away:
      ui->_status->setCurrentIndex(2);
      break;
    case Presence::XA:
      ui->_status->setCurrentIndex(3);
      break;
    case Presence::DND:
      ui->_status->setCurrentIndex(4);
      break;
    case Presence::Unavailable:
      ui->_status->setCurrentIndex(5);
      break;
    default:
      break;
  }
  ui->_status->blockSignals(false);
}
bool AdmXmppWidget::handleSubscriptionRequest (const JID &jid, const std::string &msg)
{
  Q_UNUSED(msg)

  // Automatically allow for now
  // TODO: Make this a preference variable
  // TODO: Get the preference value and if false, then present to the
  // TODO: user the opportunity to allow or block the subscription
  // TODO: request.
  _client->rosterManager()->ackSubscriptionRequest(jid, true);

  // Now subscribe to the user that subscribed to me
  _client->rosterManager()->subscribe(jid, jid.username(), StringList(), "Auto Subscribe");

  return true; // This is ignored in asynchronous mode
}
bool AdmXmppWidget::handleUnsubscriptionRequest (const JID &jid, const std::string &msg)
{
  Q_UNUSED(jid)
  Q_UNUSED(msg)

  // Unsubsribe asynchronously
  _client->rosterManager()->unsubscribe(jid, msg);
  return true; // This is ignored in asynchronous mode
}
void AdmXmppWidget::handleNonrosterPresence (const Presence &presence)
{
  Q_UNUSED(presence)
  qWarning() << QString("GlooxRosterListener::handleNonrosterPresence: UNHANDLED: %1: %2 (%3)")
              .arg(QString::fromUtf8(presence.from().username().data()))
              .arg(presence.presence())
              .arg(QString::fromUtf8(presence.status().data()));
}
void AdmXmppWidget::handleRosterError (const IQ &iq)
{
  Q_UNUSED(iq)
  qWarning() << QString("GlooxRosterListener::handleRosterError: UNHANDLED: %1").arg(iq.subtype());
}

void AdmXmppWidget::handleMessageSession(MessageSession *session)
{
  QString jid = QString::fromUtf8(session->target().bare().data());
  if(!_mapSessions->contains(jid))
  {
    _mapSessions->insert(jid,session);
    AdmXmppChatWidget *w = new AdmXmppChatWidget(session,this);
    w->setSelfJid(_client->jid());
    w->setMessageSession(session);

    _mapChats->insert(jid,w);
    connect(w,    SIGNAL(destroying(AdmXmppChatWidget*)),
            this, SLOT(sDestroyingChatWidget(AdmXmppChatWidget*)));
    connect(w,    SIGNAL(attention(AdmXmppChatWidget*)),
            this, SLOT(sAttentionChatWidget(AdmXmppChatWidget*)));

    ui->_conversations->addTab(w,
                               QIcon(),
                               QString::fromUtf8(session->target().username().data()));
  }
}

void AdmXmppWidget::_connect()
{
  QSettings set;
  set.beginGroup("XMPP");
  if(NULL == _client)
  {
    QString username = QString("%1@%2")
                       .arg(set.value(XmppPref::getName(XmppPref::userName)).toString())
                       .arg(set.value(XmppPref::getName(XmppPref::realm)).toString());
    QString password = set.value(XmppPref::getName(XmppPref::secPass)).toString();

    JID jid(username.toUtf8().constData());
    _client = new Client(jid,password.toUtf8().constData());

    // Setup the jabber disco features
    // Let others know that this is a standard full-GUI client
    _client->disco()->setIdentity("client","pc");
    // Accepts and sends the "chat states" feature
    _client->disco()->addFeature(gloox::XMLNS_CHAT_STATES);
    // Accepts and sends the "attention" feature"
    _client->disco()->addFeature(gloox::XMLNS_ATTENTION);

    _client->registerStanzaExtension(new Attention());
    _client->registerStanzaExtension(new ChatState(ChatStateActive));
    _client->registerStanzaExtension(new ChatState(ChatStateComposing));
    _client->registerStanzaExtension(new ChatState(ChatStatePaused));
    _client->registerStanzaExtension(new ChatState(ChatStateInactive));
    _client->registerStanzaExtension(new ChatState(ChatStateGone));
    _client->registerStanzaExtension(new ChatState(ChatStateInvalid));

    _client->registerConnectionListener(this);

    //register as asynchronous
    _client->rosterManager()->registerRosterListener(this, false);

    _client->registerMessageSessionHandler(this);

    QString host = set.value(XmppPref::getName(XmppPref::host)).toString();
    int port = -1;
    if(set.contains(XmppPref::getName(XmppPref::host)))
      port = set.value(XmppPref::getName(XmppPref::port)).toInt();
    if(port <= 0)
      port = -1;
    _conn = new ConnectionTCPClient( _client, _client->logInstance(), host.toUtf8().constData(), port);
    _client->setConnectionImpl(_conn);

    //connect as asynchronous
    _client->connect(false);
    _socketNotify = new QSocketNotifier(_conn->socket(), QSocketNotifier::Read, this);
    connect(_socketNotify, SIGNAL(activated(int)),
            this,   SLOT(sSockectActivated(int)));

  }
}
AdmXmppBuddyWidget * AdmXmppWidget::_addBuddyIfNotExist(const JID &jid, bool resortItems)
{
  AdmXmppBuddyWidget *buddy = NULL;
  if(_mapBuddies->contains(jid))
  {
    buddy = _mapBuddies->value(jid);
  } else {
    qDebug() << "Add buddy:" << QString::fromUtf8(jid.username().data());

    AdmXmppBuddyItem *item = new AdmXmppBuddyItem(ui->_buddyList,jid);
    _mapBuddyItems->insert(jid,item);

    buddy = item->getBuddyWidget();
    _mapBuddies->insert(jid,buddy);

    connect(buddy,  SIGNAL(destroying(AdmXmppBuddyWidget*)),
            this,   SLOT(sDestroyingBuddy(AdmXmppBuddyWidget*)));
    connect(buddy,  SIGNAL(sigRemoveBuddy(AdmXmppBuddyWidget*)),
            this,   SLOT(sRemovingBuddy(AdmXmppBuddyWidget*)));
    connect(buddy,  SIGNAL(sigSetBuddyName(AdmXmppBuddyWidget*,QString)),
            this,   SLOT(sSetBuddyName(AdmXmppBuddyWidget*,QString)));
    connect(buddy,  SIGNAL(sigBuddyNameChanged(AdmXmppBuddyWidget*,QString)),
            this,   SLOT(sBuddyNameChanged(AdmXmppBuddyWidget*,QString)));

    if(resortItems)
      ui->_buddyList->sortItems();
  }
  return buddy;
}

void AdmXmppWidget::sSockectActivated(int fd)
{
  Q_UNUSED(fd);
  ConnectionError e = _client->recv();
  if(e != ConnNoError)
  {
    qCritical() << QString("AdmXmppWidget::sSockectActivated: ConnectionError: %1")
                   .arg(e);
    qCritical() << "AdmXmppWidget::sSockectActivated: UNHANDLED CODE";
  }
}

void AdmXmppWidget::sStatusIndexChanged(int index)
{
  Presence::PresenceType _prsnce;
  if(NULL != _client)
  {
    switch(index)
    {
      case 0:
        _prsnce = Presence::Available;
        break;
      case 1:
        _prsnce = Presence::Chat;
        break;
      case 2:
        _prsnce = Presence::Away;
        break;
      case 3:
        _prsnce = Presence::XA;
        break;
      case 4:
        _prsnce = Presence::DND;
        break;
      case 5:
        _prsnce = Presence::Unavailable;
        break;
      default:
        return;
    }
    _client->setPresence(_prsnce,0,std::string());
  }
}

void AdmXmppWidget::sBuddyActivated(const QModelIndex &index)
{
  Q_UNUSED(index)

  QWidget *w = ui->_buddyList->itemWidget(ui->_buddyList->currentItem());
  AdmXmppBuddyWidget *b = qobject_cast<AdmXmppBuddyWidget*>(w);
  if(b && b->getJid() == true)
  {
    QString jidStr = QString::fromUtf8(b->getJid().bare().data());
    AdmXmppChatWidget *wchat = NULL;
    if(_mapSessions->contains(jidStr) )
    { 
      wchat = _mapChats->value(jidStr);
    } else {
      MessageSession *session = new MessageSession(_client, b->getJid());
      handleMessageSession(session);
      wchat = _mapChats->value(jidStr);
    }
    if(NULL != wchat)
    {
      ui->_conversations->setCurrentWidget(wchat);
      wchat->focusChatText();
    }
  }
}

void AdmXmppWidget::sDestroyingBuddy(AdmXmppBuddyWidget *buddy)
{
  if(_mapBuddies->contains(buddy->getJid()))
    _mapBuddies->remove(buddy->getJid());

  //ui->_buddyList->removeItemWidget();
  for(int i =0; i < ui->_buddyList->count(); ++i)
  {
    if(buddy == ui->_buddyList->itemWidget(ui->_buddyList->item(i)))
    {
      if(_mapBuddyItems->contains(buddy->getJid()))
        _mapBuddyItems->remove(buddy->getJid());
       ui->_buddyList->takeItem(i);
    }
  }
}
void AdmXmppWidget::sSetBuddyName(AdmXmppBuddyWidget *buddy, const QString &buddyName)
{
  RosterItem *rosterItem = _client->rosterManager()->getRosterItem(buddy->getJid());
  rosterItem->setName(buddyName.toStdString());
  _client->rosterManager()->synchronize();
  rosterItem = NULL;
}
void AdmXmppWidget::sBuddyNameChanged(AdmXmppBuddyWidget *buddy, const QString &buddyName)
{
  Q_UNUSED(buddy)
  Q_UNUSED(buddyName)
  qWarning() << "AdmXmppWidget::sBuddyNameChanged: UNHANDLED";
  ui->_buddyList->sortItems();
}

void AdmXmppWidget::sRemovingBuddy(AdmXmppBuddyWidget *buddy)
{
  qDebug() << "AdmXmppWidget::sRemovingBuddy";
  _client->rosterManager()->remove(buddy->getJid());
  _client->rosterManager()->synchronize();
}

void AdmXmppWidget::sDestroyingChatWidget(AdmXmppChatWidget* obj)
{
  int index = _mapChats->values().indexOf(obj);
  if(index != -1)
    _mapChats->remove(_mapChats->keys().at(index));

  if(obj->messageSession())
  {
    index = _mapSessions->values().indexOf(obj->messageSession());
    if(index != -1)
      _mapSessions->remove(_mapSessions->keys().at(index));

    _client->disposeMessageSession(obj->messageSession());
  }
}
void AdmXmppWidget::sAttentionChatWidget(AdmXmppChatWidget* obj)
{
  Q_UNUSED(obj)
  for(int i = 0; i < ui->_conversations->count(); ++i)
  {
    if(ui->_conversations->widget(i) == obj)
    {
      if(ui->_conversations->tabIcon(i).isNull()){
        ui->_conversations->setTabIcon(i,QIcon(QPixmap(":/icons/im-jabber.png")));
        ui->_conversations->setTabNeedAttention(i,true);
      }
      break;
    }
  }
}

void AdmXmppWidget::sConversationsCurrentChanged(int index)
{
  if(index == -1)
    return;

  if(!ui->_conversations->tabIcon(index).isNull())
  {
    ui->_conversations->setTabIcon(index, QIcon());
    ui->_conversations->setTabNeedAttention(index,false);
  }

  ui->_conversations->widget(index)->update();
}

void AdmXmppWidget::sConversationsTabClosed(int index)
{
  if(index == -1)
    return;
  AdmXmppChatWidget *cw = qobject_cast<AdmXmppChatWidget*>(ui->_conversations->widget(index));
  if(cw)
  {
    cw->deleteLater();
  }
}
