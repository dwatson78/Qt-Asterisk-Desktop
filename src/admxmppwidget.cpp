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
  _mapSessions = new QMap<QString,MessageSession*>();
  _mapChats = new QMap<QString,AdmXmppChatWidget*>();

  connect(ui->_status,  SIGNAL(currentIndexChanged(int)),
          this,         SLOT(sStatusIndexChanged(int))
  );

  connect(ui->_buddyList, SIGNAL(activated(QModelIndex)),
          this,           SLOT(sBuddyActivated(QModelIndex)));

  _connect();
}

AdmXmppWidget::~AdmXmppWidget()
{
  delete _client;
  delete ui;
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
        errMsg.append(QString("\nText: %1").arg(QString::fromStdString(_client->streamErrorText())));
      if(!_client->streamErrorCData().empty())
        errMsg.append(QString("\nCData: %1").arg(QString::fromStdString(_client->streamErrorCData())));
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
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemAdded: %1")
              .arg(QString::fromStdString(jid.full()));
}
void AdmXmppWidget::handleItemSubscribed (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemSubscribed: %1")
              .arg(QString::fromStdString(jid.full()));
}
void AdmXmppWidget::handleItemRemoved (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemRemoved: %1")
              .arg(QString::fromStdString(jid.full()));
}
void AdmXmppWidget::handleItemUpdated (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemUpdated: %1")
              .arg(QString::fromStdString(jid.full()));
}
void AdmXmppWidget::handleItemUnsubscribed (const JID &jid)
{
  Q_UNUSED(jid)
  qDebug() << QString("GlooxRosterListener::handleItemUnsubscribed: %1")
              .arg(QString::fromStdString(jid.full()));
}
void AdmXmppWidget::handleRoster (const Roster &roster)
{
  Q_UNUSED(roster)
  qDebug() << QString("GlooxRosterListener::handleRoster");


  std::map<const std::string, RosterItem*>::const_iterator it;
  QListWidgetItem *item = NULL;
  AdmXmppBuddyWidget *widget = NULL;
  for(it = roster.begin(); it != roster.end(); ++it)
  {
    qDebug() << QString::fromStdString(it->first);
    qDebug() << QString("Online? %1").arg(it->second->online());
    item = new QListWidgetItem(ui->_buddyList);
    widget = new AdmXmppBuddyWidget(ui->_buddyList);
    _mapBuddies->insert(it->second->jidJID(), widget);
    connect(widget,  SIGNAL(destroying(AdmXmppBuddyWidget*)),
            this,   SLOT(sDestroyingBuddy(AdmXmppBuddyWidget*)));
    ui->_buddyList->addItem(item);
    ui->_buddyList->setItemWidget(item,widget);
    item->setSizeHint(widget->sizeHint());
    item = NULL;
    widget = NULL;
  }
  //TODO: Add all roster elements now!


}
void AdmXmppWidget::handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg)
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
  AdmXmppBuddyWidget *buddy = NULL;
  if(_mapBuddies->contains(item.jidJID()))
  {
    buddy = _mapBuddies->value(item.jidJID());
  } else {
    buddy = new AdmXmppBuddyWidget();
    _mapBuddies->insert(item.jidJID(),buddy);
    connect(buddy,  SIGNAL(destroying(AdmXmppBuddyWidget*)),
            this,   SLOT(sDestroyingBuddy(AdmXmppBuddyWidget*)));

    //ui->_layoutBuddies->addWidget(buddy);
  }
  buddy->setPresence(item, QString::fromStdString(resource), presence, QString::fromStdString(msg));
}
void AdmXmppWidget::handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg)
{
  Q_UNUSED(item)
  Q_UNUSED(resource)
  Q_UNUSED(presence)
  Q_UNUSED(msg)
  qDebug() << QString("GlooxRosterListener::handleSelfPresence: %1")
              .arg(QString::fromStdString(msg));
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
  Q_UNUSED(jid)
  Q_UNUSED(msg)

  qDebug() << "handleSubscriptionRequest";

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

  qDebug() << "handleUnsubscriptionRequest";

  // Unsubsribe asynchronously
  _client->rosterManager()->unsubscribe(jid, msg);
  return true; // This is ignored in asynchronous mode
}
void AdmXmppWidget::handleNonrosterPresence (const Presence &presence)
{
  Q_UNUSED(presence)
  qDebug() << QString("GlooxRosterListener::handleNonrosterPresence : %1:  %2 (%3)")
              .arg(QString::fromStdString(presence.from().username()))
              .arg(presence.presence())
              .arg(QString::fromStdString(presence.status()));
}
void AdmXmppWidget::handleRosterError (const IQ &iq)
{
  Q_UNUSED(iq)
  qDebug() << QString("GlooxRosterListener::handleRosterError : %1").arg(iq.subtype());
}

void AdmXmppWidget::handleMessageSession(MessageSession *session)
{
  if(!_mapSessions->contains(QString::fromStdString(session->threadID())))
  {
    _mapSessions->insert(QString::fromStdString(session->threadID()),session);
    AdmXmppChatWidget *w = new AdmXmppChatWidget(session,this);
    _mapChats->insert(QString::fromStdString(session->threadID()),w);
    connect(w,    SIGNAL(destroying(AdmXmppChatWidget*)),
            this, SLOT(sDestroyingChatWidget(AdmXmppChatWidget*)));
    ui->_conversations->addTab(w,
                               QIcon(),
                               QString::fromUtf8(session->target().username().data()));
    w->setMessageSession(session);
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

    _client->disco()->addFeature(gloox::XMLNS_CHAT_STATES);
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

void AdmXmppWidget::sSockectActivated(int fd)
{
  Q_UNUSED(fd);
  ConnectionError e = _client->recv();
  if(e != ConnNoError)
    qDebug() << "Connection Error: " << e;
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
        _prsnce =  Presence::Unavailable;
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
  if(b)
  {
    //AdmXmppChatWidget *w = new AdmXmppChatWidget(this);

    //ui->_conversations->addTab(w,QIcon(),QString::fromStdString(b->getJid().username()));
  }
}

void AdmXmppWidget::sDestroyingBuddy(AdmXmppBuddyWidget *buddy)
{
  _mapBuddies->remove(buddy->getJid());
}

void AdmXmppWidget::sDestroyingChatWidget(AdmXmppChatWidget* obj)
{
  qDebug() << "AdmXmppWidget::sDestroyingChatWidget";
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
