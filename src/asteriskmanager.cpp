/*
This file was originally authored by

Rudi Lee
rudilee
Kanaya Hijab
Jakarta, Indonesia
rudi@kanayahijab.com

and is distrubuted under the MIT license
*/

#include <QUuid>

#include "asteriskmanager.h"

AsteriskManager::AsteriskManager(QObject *parent) :
  QTcpSocket(parent),
  responseEnum(AsteriskManager::staticMetaObject.enumerator(AsteriskManager::staticMetaObject.indexOfEnumerator("Response"))),
  eventEnum(AsteriskManager::staticMetaObject.enumerator(AsteriskManager::staticMetaObject.indexOfEnumerator("Event")))
{
  connect(this, SIGNAL(readyRead()), SLOT(onReadyRead()));
}

QString AsteriskManager::eventValue(AsteriskManager::Event event)
{
  return eventEnum.valueToKey(event);
}

QString AsteriskManager::actionAbsoluteTimeout()
{
  return sendAction("AbsoluteTimeout");
}

QString AsteriskManager::actionAgentLogoff()
{
  return sendAction("AgentLogoff");
}

QString AsteriskManager::actionAgents()
{
  return sendAction("Agents");
}

QString AsteriskManager::actionAGI()
{
  return sendAction("AGI");
}

QString AsteriskManager::actionAOCMessage()
{
  return sendAction("AOCMessage");
}

QString AsteriskManager::actionAtxfer()
{
  return sendAction("Atxfer");
}

QString AsteriskManager::actionBridge()
{
  return sendAction("Bridge");
}

QString AsteriskManager::actionChallenge()
{
  return sendAction("Challenge");
}

QString AsteriskManager::actionChangeMonitor()
{
  return sendAction("ChangeMonitor");
}

QString AsteriskManager::actionCommand()
{
  return sendAction("Command");
}

QString AsteriskManager::actionConfbridgeKick(uint conference, QString channel)
{
  QVariantMap headers;
  headers["Conference"] = conference;
  headers["Channel"] = channel;

  return sendAction("ConfbridgeKick", headers);
}

QString AsteriskManager::actionConfbridgeList()
{
  return sendAction("ConfbridgeList");
}

QString AsteriskManager::actionConfbridgeListRooms()
{
  return sendAction("ConfbridgeListRooms");
}

QString AsteriskManager::actionConfbridgeLock()
{
  return sendAction("ConfbridgeLock");
}

QString AsteriskManager::actionConfbridgeMute(uint conference, QString channel)
{
  QVariantMap headers;
  headers["Conference"] = conference;
  headers["Channel"] = channel;

  return sendAction("ConfbridgeMute", headers);
}

QString AsteriskManager::actionConfbridgeSetSingleVideoSrc()
{
  return sendAction("ConfbridgeSetSingleVideoSrc");
}

QString AsteriskManager::actionConfbridgeStartRecord()
{
  return sendAction("ConfbridgeStartRecord");
}

QString AsteriskManager::actionConfbridgeStopRecord()
{
  return sendAction("ConfbridgeStopRecord");
}

QString AsteriskManager::actionConfbridgeUnlock()
{
  return sendAction("ConfbridgeUnlock");
}

QString AsteriskManager::actionConfbridgeUnmute(uint conference, QString channel)
{
  QVariantMap headers;
  headers["Conference"] = conference;
  headers["Channel"] = channel;

  return sendAction("ConfbridgeUnmute", headers);
}

QString AsteriskManager::actionCoreSettings()
{
  return sendAction("CoreSettings");
}

QString AsteriskManager::actionCoreShowChannels()
{
  return sendAction("CoreShowChannels");
}

QString AsteriskManager::actionCoreStatus()
{
  return sendAction("CoreStatus");
}

QString AsteriskManager::actionCreateConfig()
{
  return sendAction("CreateConfig");
}

QString AsteriskManager::actionDAHDIDialOffhook()
{
  return sendAction("DAHDIDialOffhook");
}

QString AsteriskManager::actionDAHDIDNDoff()
{
  return sendAction("DAHDIDNDoff");
}

QString AsteriskManager::actionDAHDIDNDon()
{
  return sendAction("DAHDIDNDon");
}

QString AsteriskManager::actionDAHDIHangup()
{
  return sendAction("DAHDIHangup");
}

QString AsteriskManager::actionDAHDIRestart()
{
  return sendAction("DAHDIRestart");
}

QString AsteriskManager::actionDAHDIShowChannels()
{
  return sendAction("DAHDIShowChannels");
}

QString AsteriskManager::actionDAHDITransfer()
{
  return sendAction("DAHDITransfer");
}

QString AsteriskManager::actionDataGet()
{
  return sendAction("DataGet");
}

QString AsteriskManager::actionDBDel()
{
  return sendAction("DBDel");
}

QString AsteriskManager::actionDBDelTree()
{
  return sendAction("DBDelTree");
}

QString AsteriskManager::actionDBGet(const QString &family, const QString &key)
{
  QVariantMap headers;
  headers["Family"] = family;
  headers["Key"] = key;
  return sendAction("DBGet",headers);
}

QString AsteriskManager::actionDBPut()
{
  return sendAction("DBPut");
}

QString AsteriskManager::actionEvents()
{
  return sendAction("Events");
}

QString AsteriskManager::actionExtensionState(const QString &exten, const QString &context)
{
  QVariantMap headers;
  headers["Exten"] = exten;
  if(!context.isNull() && context.isEmpty())
    headers["Context"] = context;

  return sendAction("ExtensionState", headers);
}

QString AsteriskManager::actionFilter()
{
  return sendAction("Filter");
}

QString AsteriskManager::actionFilterList()
{
  return sendAction("FilterList");
}

QString AsteriskManager::actionGetConfig()
{
  return sendAction("GetConfig");
}

QString AsteriskManager::actionGetConfigJSON()
{
  return sendAction("GetConfigJSON");
}

QString AsteriskManager::actionGetvar()
{
  return sendAction("Getvar");
}

QString AsteriskManager::actionHangup(QString channel, uint cause)
{
  QVariantMap headers;
  headers["Channel"] = channel;

  insertNotEmpty(&headers, "Cause", cause);

    return sendAction("Hangup", headers);
}

QString AsteriskManager::actionHold(QString channel, boolean hold)
{
    QVariantMap headers;
    headers["Channel"] = channel;

    insertNotEmpty(&headers, "Hold", hold);

    return sendAction("Hold", headers);
}

QString AsteriskManager::actionIAXnetstats()
{
  return sendAction("IAXnetstats");
}

QString AsteriskManager::actionIAXpeerlist()
{
  return sendAction("IAXpeerlist");
}

QString AsteriskManager::actionIAXpeers()
{
  return sendAction("IAXpeers");
}

QString AsteriskManager::actionIAXregistry()
{
  return sendAction("IAXregistry");
}

QString AsteriskManager::actionJabberSend()
{
  return sendAction("JabberSend");
}

QString AsteriskManager::actionJabberSendResJabber()
{
  return sendAction("JabberSend_res_jabber");
}

QString AsteriskManager::actionJabberSendResXmpp()
{
  return sendAction("JabberSend_res_xmpp");
}

QString AsteriskManager::actionListCategories()
{
  return sendAction("ListCategories");
}

QString AsteriskManager::actionListCommands()
{
  return sendAction("ListCommands");
}

QString AsteriskManager::actionLocalOptimizeAway()
{
  return sendAction("LocalOptimizeAway");
}

QString AsteriskManager::actionLogin(QString username, QString secret)
{
  QVariantMap headers;
  headers["Username"] = username;

  insertNotEmpty(&headers, "Secret", secret);

  return sendAction("Login", headers);
}

QString AsteriskManager::actionLogoff()
{
  return sendAction("Logoff");
}

QString AsteriskManager::actionMailboxCount()
{
  return sendAction("MailboxCount");
}

QString AsteriskManager::actionMailboxStatus()
{
  return sendAction("MailboxStatus");
}

QString AsteriskManager::actionMeetmeList()
{
  return sendAction("MeetmeList");
}

QString AsteriskManager::actionMeetmeListRooms()
{
  return sendAction("MeetmeListRooms");
}

QString AsteriskManager::actionMeetmeMute()
{
  return sendAction("MeetmeMute");
}

QString AsteriskManager::actionMeetmeUnmute()
{
  return sendAction("MeetmeUnmute");
}

QString AsteriskManager::actionMessageSend()
{
  return sendAction("MessageSend");
}

QString AsteriskManager::actionMixMonitor()
{
  return sendAction("MixMonitor");
}

QString AsteriskManager::actionMixMonitorMute()
{
  return sendAction("MixMonitorMute");
}

QString AsteriskManager::actionModuleCheck()
{
  return sendAction("ModuleCheck");
}

QString AsteriskManager::actionModuleLoad()
{
  return sendAction("ModuleLoad");
}

QString AsteriskManager::actionMonitor()
{
  return sendAction("Monitor");
}

QString AsteriskManager::actionMuteAudio()
{
  return sendAction("MuteAudio");
}

QString AsteriskManager::actionOriginate(QString channel,
                     QString exten,
                     QString context,
                     uint priority,
                     QString application,
                     QString data,
                     uint timeout,
                     QString callerID,
                     QVariantMap variables,
                     QString account,
                     boolean earlyMedia,
                     boolean async,
                     QStringList codecs)
{
  QVariantMap headers;
  headers["Channel"] = channel;

  insertNotEmpty(&headers, "Exten", exten);
  insertNotEmpty(&headers, "Context", context);
  insertNotEmpty(&headers, "Priority", priority);
  insertNotEmpty(&headers, "Application", application);
  insertNotEmpty(&headers, "Data", data);
  insertNotEmpty(&headers, "Timeout", timeout);
  insertNotEmpty(&headers, "CallerID", callerID);
  insertNotEmpty(&headers, "Account", account);
  insertNotEmpty(&headers, "EarlyMedia", earlyMedia);
  insertNotEmpty(&headers, "Async", async);
  insertNotEmpty(&headers, "Codecs", codecs.join(","));

  if (!variables.isEmpty()) {
    QMapIterator<QString, QVariant> variable(variables);
    while (variable.hasNext()) {
      variable.next();

      headers.insertMulti("Variable", QString("%1=%2").arg(variable.key(), valueToString(variable.value())));
    }
  }

  return sendAction("Originate", headers);
}

QString AsteriskManager::actionPark(QString channel, QString channel2, uint timeout, QString parkinglot)
{
  QVariantMap headers;
  headers["Channel"] = channel;
  headers["Channel2"] = channel2;

  insertNotEmpty(&headers, "Timeout", timeout);
  insertNotEmpty(&headers, "Parkinglot", parkinglot);

  return sendAction("Park", headers);
}

QString AsteriskManager::actionParkedCalls()
{
  return sendAction("ParkedCalls");
}

QString AsteriskManager::actionParkinglots()
{
  return sendAction("Parkinglots");
}

QString AsteriskManager::actionPauseMonitor()
{
  return sendAction("PauseMonitor");
}

QString AsteriskManager::actionPing()
{
  return sendAction("Ping");
}

QString AsteriskManager::actionPlayDTMF(QString channel, QString digit)
{
  QVariantMap headers;
  headers["Channel"] = channel;
  headers["Digit"] = digit;

  return sendAction("PlayDTMF", headers);
}

QString AsteriskManager::actionPresenceState()
{
  return sendAction("PresenceState");
}

QString AsteriskManager::actionPRIShowSpans()
{
  return sendAction("PRIShowSpans");
}

QString AsteriskManager::actionQueueAdd(QString queue,
                    QString interface,
                    uint penalty,
                    boolean paused,
                    QString memberName,
                    QString stateInterface)
{
  QVariantMap headers;
  headers["Queue"] = queue;
  headers["Interface"] = interface;

  insertNotEmpty(&headers, "Penalty", penalty);
  insertNotEmpty(&headers, "Paused", paused);
  insertNotEmpty(&headers, "MemberName", memberName);
  insertNotEmpty(&headers, "StateInterface", stateInterface);

  return sendAction("QueueAdd", headers);
}

QString AsteriskManager::actionQueueLog()
{
  return sendAction("QueueLog");
}

QString AsteriskManager::actionQueueMemberRingInUse()
{
  return sendAction("QueueMemberRingInUse");
}

QString AsteriskManager::actionQueuePause(QString interface, boolean paused, QString queue, QString reason)
{
  QVariantMap headers;
  headers["Interface"] = interface;
  headers["Paused"] = paused;

    insertNotEmpty(&headers, "Queue", queue);
  insertNotEmpty(&headers, "Reason", reason);

  return sendAction("QueuePause", headers);
}

QString AsteriskManager::actionQueuePenalty()
{
  return sendAction("QueuePenalty");
}

QString AsteriskManager::actionQueueReload()
{
  return sendAction("QueueReload");
}

QString AsteriskManager::actionQueueRemove(QString queue, QString interface)
{
  QVariantMap headers;
  headers["Queue"] = queue;
  headers["Interface"] = interface;

  return sendAction("QueueRemove");
}

QString AsteriskManager::actionQueueReset()
{
  return sendAction("QueueReset");
}

QString AsteriskManager::actionQueueRule()
{
  return sendAction("QueueRule");
}

QString AsteriskManager::actionQueues()
{
  return sendAction("Queues");
}

QString AsteriskManager::actionQueueStatus(QString queue, QString member)
{
  QVariantMap headers;

  insertNotEmpty(&headers, "Queue", queue);
  insertNotEmpty(&headers, "Member", member);

  return sendAction("QueueStatus", headers);
}

QString AsteriskManager::actionQueueSummary()
{
  return sendAction("QueueSummary");
}

QString AsteriskManager::actionRedirect(QString channel,
                    QString exten,
                    QString context,
                    uint priority,
                    QString extraChannel,
                    QString extraExten,
                    QString extraContext,
                    uint extraPriority)
{
  QVariantMap headers;
  headers["Channel"] = channel;
  headers["Exten"] = exten;
  headers["Context"] = context;
  headers["Priority"] = priority;

  insertNotEmpty(&headers, "ExtraChannel", extraChannel);
  insertNotEmpty(&headers, "ExtraExten", extraExten);
  insertNotEmpty(&headers, "ExtraContext", extraContext);
  insertNotEmpty(&headers, "ExtraPriority", extraPriority);

  return sendAction("Redirect", headers);
}

QString AsteriskManager::actionReload()
{
  return sendAction("Reload");
}

QString AsteriskManager::actionSendText()
{
  return sendAction("SendText");
}

QString AsteriskManager::actionSetvar(const QString &channel, const QString &variable, const QString &value)
{
  QVariantMap headers;
  headers["Channel"] = channel;
  headers["Variable"] = variable;
  headers["Value"] = value;
  return sendAction("Setvar",headers);
}

QString AsteriskManager::actionShowDialPlan()
{
  return sendAction("ShowDialPlan");
}

QString AsteriskManager::actionSIPnotify()
{
  return sendAction("SIPnotify");
}

QString AsteriskManager::actionSIPpeers()
{
  return sendAction("SIPpeers");
}

QString AsteriskManager::actionSIPpeerstatus()
{
  return sendAction("SIPpeerstatus");
}

QString AsteriskManager::actionSIPqualifypeer()
{
  return sendAction("SIPqualifypeer");
}

QString AsteriskManager::actionSIPshowpeer(QString peer)
{
  QVariantMap headers;
  headers["Peer"] = peer;

  return sendAction("SIPshowpeer", headers);
}

QString AsteriskManager::actionSIPshowregistry()
{
  return sendAction("SIPshowregistry");
}

QString AsteriskManager::actionSKINNYdevices()
{
  return sendAction("SKINNYdevices");
}

QString AsteriskManager::actionSKINNYlines()
{
  return sendAction("SKINNYlines");
}

QString AsteriskManager::actionSKINNYshowdevice()
{
  return sendAction("SKINNYshowdevice");
}

QString AsteriskManager::actionSKINNYshowline()
{
  return sendAction("SKINNYshowline");
}

QString AsteriskManager::actionStatus()
{
  return sendAction("Status");
}

QString AsteriskManager::actionStopMixMonitor()
{
  return sendAction("StopMixMonitor");
}

QString AsteriskManager::actionStopMonitor()
{
  return sendAction("StopMonitor");
}

QString AsteriskManager::actionUnpauseMonitor()
{
  return sendAction("UnpauseMonitor");
}

QString AsteriskManager::actionUpdateConfig()
{
  return sendAction("UpdateConfig");
}

QString AsteriskManager::actionUserEvent()
{
  return sendAction("UserEvent");
}

QString AsteriskManager::actionVoicemailUsersList()
{
  return sendAction("VoicemailUsersList");
}

QString AsteriskManager::actionWaitEvent()
{
  return sendAction("WaitEvent");
}

QString AsteriskManager::valueToString(QVariant value)
{
  switch (value.type()) {
  case QVariant::Char:
    return value.toChar() == 1 ? "true" : "false";
  default:
    return value.toString();
  }
}

QVariant AsteriskManager::stringValue(QString string)
{
  QVariant value(string);
  if (string == "true" || string == "false")
    value.setValue(string == "true");
  else if (QVariant(value).convert(QVariant::UInt))
  {
    bool ok = false;
    value.setValue(string.toUInt(&ok));
    if(!ok)
      value = QVariant(string);
  }
  return value;
}

void AsteriskManager::insertNotEmpty(QVariantMap *headers, QString key, QVariant value)
{
  bool isEmpty = false;

  switch (value.type()) {
  case QMetaType::Int:
    isEmpty = value.toChar() == -1;
    break;
  case QMetaType::UInt:
    isEmpty = value.toUInt() == 0;
    break;
  default:
    isEmpty = value.isNull();
    break;
  }

  if (!isEmpty)
    headers->insert(key, value);
}

void AsteriskManager::dispatchPacket()
{
  if (packetBuffer.contains("Response")) {
    QString actionID = packetBuffer["ActionID"].toString();
    Response response = (Response) responseEnum.keyToValue(packetBuffer["Response"].toByteArray().data());

    emit responseSent(response, packetBuffer, actionID);
  } else if (packetBuffer.contains("Event")) {
    Event event = (Event) eventEnum.keyToValue(packetBuffer["Event"].toByteArray().data());

    emit eventGenerated(event, packetBuffer);
  }

  packetBuffer.clear();
}

QString AsteriskManager::sendAction(QString action, QVariantMap headers)
{
  QString actionID = QString();

  if (state() == QAbstractSocket::ConnectedState) {
    actionID = QUuid::createUuid().toString();

    headers["ActionID"] = actionID;
    headers["Action"] = action;

    QMapIterator<QString, QVariant> header(headers);
    while (header.hasNext()) {
      header.next();

      write(QString("%1: %2\r\n").arg(header.key(), valueToString(header.value())).toLatin1());
    }

    write("\r\n");
  }

  return actionID;
}

void AsteriskManager::onReadyRead()
{
  QRegExp keyValue("^([A-Za-z0-9\\-]+):\\s(.+)$");
  QByteArray line;


  while (canReadLine()) {
    line = readLine();

    if (line != "\r\n") {
      if (keyValue.indexIn(line) > -1)
        packetBuffer[keyValue.cap(1)] = stringValue(keyValue.cap(2).trimmed());
      else if (line.startsWith("ChanVariable"))
      {
        if(!packetBuffer.contains("ChanVars"))
        {
          QMap<QString, QVariant> chanvars;
          packetBuffer.insert("ChanVars",chanvars);
        }
        QMap<QString, QVariant> chanvars = packetBuffer.value("ChanVars").toMap();
        //Get the Channel Name
        QRegExp re ("^ChanVariable\\(([^)]+)\\):\\ (.*)$");
        if(re.exactMatch(line))
        {
          if(!chanvars.contains(re.cap(1)))
          {
            QMap<QString, QVariant> chanvarvals;
            chanvars.insert(re.cap(1),chanvarvals);
          }
          QMap<QString, QVariant> chanvarvals = chanvars.value(re.cap(1)).toMap();
          QRegExp re2 ("^([^=]+)=(.*)$");
          if(re2.exactMatch(re.cap(2).trimmed()))
          {
            if(re2.captureCount() == 2)
            {
              chanvarvals.insert(re2.cap(1), re2.cap(2));
            }
          }
          chanvars.insert(re.cap(1),chanvarvals);
        }
        packetBuffer.insert("ChanVars",chanvars);
      }
      else if (line.startsWith("Asterisk Call Manager"))
        emit connected(line.replace("Asterisk Call Manager/", QByteArray()).trimmed());
    } else if (!packetBuffer.isEmpty()) {
      dispatchPacket();
    }
  }
}
