/*
This file was originally authored by

Rudi Lee
rudilee
Kanaya Hijab
Jakarta, Indonesia
rudi@kanayahijab.com

and is distrubuted under the MIT license
*/

#ifndef ASTERISKMANAGER_H
#define ASTERISKMANAGER_H

#include <QTcpSocket>
#include <QMetaEnum>
#include <QStringList>

#define yes "yes"
#define no "no"

typedef int boolean;
typedef QString answer;

class AsteriskManager : public QTcpSocket
{
  Q_OBJECT

  Q_ENUMS(Response)
  Q_ENUMS(Event)

public:
  enum Response {
    Success,
    Error,
    Follows
  };

  enum Event {
    AgentCalled,
    AgentComplete,
    AgentConnect,
    AgentDump,
    AgentRingNoAnswer,
    Agentlogin,
    Agentlogoff,
    Alarm,
    AlarmClear,
    Bridge,
    BridgeAction,
    BridgeExec,
    ChanSpyStart,
    ChanSpyStop,
    ConfbridgeEnd,
    ConfbridgeJoin,
    ConfbridgeLeave,
    ConfbridgeStart,
    ConfbridgeTalking,
    DAHDIChannel,
    DNDState,
    DTMF,
    Dial,
    ExtensionStatus,
    FullyBooted,
    Hangup,
    HangupHandlerPop,
    HangupHandlerPush,
    HangupHandlerRun,
    HangupRequest,
    Hold,
    Join,
    Leave,
    LocalBridge,
    LogChannel,
    Masquerade,
    MeetmeEnd,
    MeetmeJoin,
    MeetmeLeave,
    MeetmeMute,
    MeetmeTalkRequest,
    MeetmeTalking,
    MessageWaiting,
    ModuleLoadReport,
    NewAccountCode,
    NewCallerid,
    NewPeerAccount,
    Newchannel,
    Newexten,
    Newstate,
    OriginateResponse,
    ParkedCall,
    ParkedCallGiveUp,
    ParkedCallTimeOut,
    Pickup,
    PresenceStatus,
    QueueCallerAbandon,
    QueueMemberAdded,
    QueueMemberPaused,
    QueueMemberPenalty,
    QueueMemberRemoved,
    QueueMemberRinginuse,
    QueueMemberStatus,
    Rename,
    Shutdown,
    SoftHangupRequest,
    SpanAlarm,
    SpanAlarmClear,
    UnParkedCall,
    VarSet,
    // Event yg gak ke dokumentasi di 'make full' asterisk
    CoreShowChannel,
    CoreShowChannelsComplete,
    MusicOnHold,
    PeerEntry,
    PeerlistComplete,
    PeerStatus,
    QueueEntry,
    QueueMember,
    QueueParams,
    QueueStatusComplete,
    RTCPReceived,
    RTCPSent
  };

  enum ExtStatus{
    Removed     = -2,
    Deactivated = -1,
    NotInUse    = 0,
    InUse       = 1 << 0,
    Busy        = 1 << 1,
    Unavailable = 1 << 2,
    Ringing     = 1 << 3,
    OnHold      = 1 << 4
  };
  Q_DECLARE_FLAGS(ExtStatuses, ExtStatus)

  explicit AsteriskManager(QObject *parent = 0);

  QString eventValue(AsteriskManager::Event event);

  QString actionAbsoluteTimeout();
  QString actionAgentLogoff();
  QString actionAgents();
  QString actionAGI();
  QString actionAOCMessage();
  QString actionAtxfer();
  QString actionBridge();
  QString actionChallenge();
  QString actionChangeMonitor();
  QString actionCommand();
  QString actionConfbridgeKick(uint conference, QString channel);
  QString actionConfbridgeList();
  QString actionConfbridgeListRooms();
  QString actionConfbridgeLock();
  QString actionConfbridgeMute(uint conference, QString channel);
  QString actionConfbridgeSetSingleVideoSrc();
  QString actionConfbridgeStartRecord();
  QString actionConfbridgeStopRecord();
  QString actionConfbridgeUnlock();
  QString actionConfbridgeUnmute(uint conference, QString channel);
  QString actionCoreSettings();
  QString actionCoreShowChannels();
  QString actionCoreStatus();
  QString actionCreateConfig();
  QString actionDAHDIDialOffhook();
  QString actionDAHDIDNDoff();
  QString actionDAHDIDNDon();
  QString actionDAHDIHangup();
  QString actionDAHDIRestart();
  QString actionDAHDIShowChannels();
  QString actionDAHDITransfer();
  QString actionDataGet();
  QString actionDBDel();
  QString actionDBDelTree();
  QString actionDBGet(const QString &family, const QString &key);
  QString actionDBPut();
  QString actionEvents();
  QString actionExtensionState();
  QString actionFilter();
  QString actionFilterList();
  QString actionGetConfig();
  QString actionGetConfigJSON();
  QString actionGetvar();
    QString actionHangup(QString channel, uint cause = 0);
    // Action Hold bukan action bawaan AMI, harus di patch dulu di manager.c
    QString actionHold(QString channel, boolean hold = true);
  QString actionIAXnetstats();
  QString actionIAXpeerlist();
  QString actionIAXpeers();
  QString actionIAXregistry();
  QString actionJabberSend();
  QString actionJabberSendResJabber();
  QString actionJabberSendResXmpp();
  QString actionListCategories();
  QString actionListCommands();
  QString actionLocalOptimizeAway();
  QString actionLogin(QString username, QString secret = QString());
  QString actionLogoff();
  QString actionMailboxCount();
  QString actionMailboxStatus();
  QString actionMeetmeList();
  QString actionMeetmeListRooms();
  QString actionMeetmeMute();
  QString actionMeetmeUnmute();
  QString actionMessageSend();
  QString actionMixMonitor();
  QString actionMixMonitorMute();
  QString actionModuleCheck();
  QString actionModuleLoad();
  QString actionMonitor();
  QString actionMuteAudio();
  QString actionOriginate(QString channel,
              QString exten = QString(),
              QString context = QString(),
              uint priority = 0,
              QString application = QString(),
              QString data = QString(),
              uint timeout = 0,
              QString callerID = QString(),
              QVariantMap variables = QVariantMap(),
              QString account = QString(),
              boolean earlyMedia = -1,
              boolean async = -1,
              QStringList codecs = QStringList());
  QString actionPark(QString channel, QString channel2, uint timeout = 0, QString parkinglot = QString());
  QString actionParkedCalls();
  QString actionParkinglots();
  QString actionPauseMonitor();
  QString actionPing();
  QString actionPlayDTMF(QString channel, QString digit);
  QString actionPresenceState();
  QString actionPRIShowSpans();
  QString actionQueueAdd(QString queue,
               QString interface,
               uint penalty = 0,
               boolean paused = -1,
               QString memberName = QString(),
               QString stateInterface = QString());
  QString actionQueueLog();
  QString actionQueueMemberRingInUse();
  QString actionQueuePause(QString interface, boolean paused, QString queue = QString(), QString reason = QString());
  QString actionQueuePenalty();
  QString actionQueueReload();
  QString actionQueueRemove(QString queue, QString interface);
  QString actionQueueReset();
  QString actionQueueRule();
  QString actionQueues();
  QString actionQueueStatus(QString queue = QString(), QString member = QString());
  QString actionQueueSummary();
  QString actionRedirect(QString channel,
               QString exten,
               QString context,
               uint priority,
               QString extraChannel = QString(),
               QString extraExten = QString(),
               QString extraContext = QString(),
               uint extraPriority = 0);
  QString actionReload();
  QString actionSendText();
  QString actionSetvar(const QString &channel, const QString &variable, const QString &value);
  QString actionShowDialPlan();
  QString actionSIPnotify();
  QString actionSIPpeers();
  QString actionSIPpeerstatus();
  QString actionSIPqualifypeer();
  QString actionSIPshowpeer(QString peer);
  QString actionSIPshowregistry();
  QString actionSKINNYdevices();
  QString actionSKINNYlines();
  QString actionSKINNYshowdevice();
  QString actionSKINNYshowline();
  QString actionStatus();
  QString actionStopMixMonitor();
  QString actionStopMonitor();
  QString actionUnpauseMonitor();
  QString actionUpdateConfig();
  QString actionUserEvent();
  QString actionVoicemailUsersList();
  QString actionWaitEvent();

private:
  QMetaEnum responseEnum, eventEnum;
  QVariantMap packetBuffer;

  QString valueToString(QVariant value);
  QVariant stringValue(QString string);
  void insertNotEmpty(QVariantMap *headers, QString key, QVariant value);
  void dispatchPacket();
  QString sendAction(QString action, QVariantMap headers = QVariantMap());

private slots:
  void onReadyRead();

signals:
  void connected(QString version);
  void responseSent(AsteriskManager::Response response, QVariantMap headers, QString actionID);
  void eventGenerated(AsteriskManager::Event event, QVariantMap headers);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(AsteriskManager::ExtStatuses)

#endif // ASTERISKMANAGER_H
