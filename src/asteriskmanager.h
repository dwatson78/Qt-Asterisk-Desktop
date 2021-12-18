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
  Q_ENUMS(ChanState)

public:
  enum Response {
    Success,
    Error,
    Follows
  };

  enum Event {
      NOOP_PLACEHOLDER
    , AgentCalled
    , AgentComplete
    , AgentConnect
    , AgentDump
    , AgentLogin
    , AgentLogoff
    , AgentRingNoAnswer
    , Agents
    , AgentsComplete
    , AGIExecEnd
    , AGIExecStart
    , Alarm
    , AlarmClear
    , AorDetail
    , AsyncAGIEnd
    , AsyncAGIExec
    , AsyncAGIStart
    , AttendedTransfer
    , AuthDetail
    , AuthMethodNotAllowed
    , BlindTransfer
    , BridgeCreate
    , BridgeDestroy
    , BridgeEnter
    , BridgeInfoChannel
    , BridgeInfoComplete
    , BridgeLeave
    , BridgeMerge
    , BridgeVideoSourceUpdate
    , Cdr
    , CEL
    , ChallengeResponseFailed
    , ChallengeSent
    , ChannelTalkingStart
    , ChannelTalkingStop
    , ChanSpyStart
    , ChanSpyStop
    , ConfbridgeEnd
    , ConfbridgeJoin
    , ConfbridgeLeave
    , ConfbridgeList
    , ConfbridgeMute
    , ConfbridgeRecord
    , ConfbridgeStart
    , ConfbridgeStopRecord
    , ConfbridgeTalking
    , ConfbridgeUnmute
    , ContactStatus
    , ContactStatusDetail
    , CoreShowChannel
    , CoreShowChannelsComplete
    , DAHDIChannel
    , DeviceStateChange
    , DeviceStateListComplete
    , DialBegin
    , DialEnd
    , DNDState
    , DTMFBegin
    , DTMFEnd
    , EndpointDetail
    , EndpointDetailComplete
    , EndpointList
    , EndpointListComplete
    , ExtensionStateListComplete
    , ExtensionStatus
    , FailedACL
    , FAXSession
    , FAXSessionsComplete
    , FAXSessionsEntry
    , FAXStats
    , FAXStatus
    , FullyBooted
    , Hangup
    , HangupHandlerPop
    , HangupHandlerPush
    , HangupHandlerRun
    , HangupRequest
    , Hold
    , IdentifyDetail
    , InvalidAccountID
    , InvalidPassword
    , InvalidTransport
    , LoadAverageLimit
    , LocalBridge
    , LocalOptimizationBegin
    , LocalOptimizationEnd
    , LogChannel
    , MCID
    , MeetmeEnd
    , MeetmeJoin
    , MeetmeLeave
    , MeetmeMute
    , MeetmeTalking
    , MeetmeTalkRequest
    , MemoryLimit
    , MessageWaiting
    , MiniVoiceMail
    , MonitorStart
    , MonitorStop
    , MusicOnHoldStart
    , MusicOnHoldStop
    , MWIGet
    , MWIGetComplete
    , NewAccountCode
    , NewCallerid
    , Newchannel
    , NewConnectedLine
    , NewExten
    , Newstate
    , OriginateResponse
    , ParkedCall
    , ParkedCallGiveUp
    , ParkedCallSwap
    , ParkedCallTimeOut
    , PeerEntry
    , PeerStatus
    , Pickup
    , PresenceStateChange
    , PresenceStateListComplete
    , PresenceStatus
    , QueueCallerAbandon
    , QueueCallerJoin
    , QueueCallerLeave
    , QueueMemberAdded
    , QueueMemberPause
    , QueueMemberPenalty
    , QueueMemberRemoved
    , QueueMemberRinginuse
    , QueueMemberStatus
    , ReceiveFAX
    , Registry
    , Reload
    , Rename
    , RequestBadFormat
    , RequestNotAllowed
    , RequestNotSupported
    , RTCPReceived
    , RTCPSent
    , SendFAX
    , SessionLimit
    , SessionTimeout
    , Shutdown
    , SIPQualifyPeerDone
    , SoftHangupRequest
    , SpanAlarm
    , SpanAlarmClear
    , Status
    , StatusComplete
    , SuccessfulAuth
    , TransportDetail
    , UnexpectedAddress
    , Unhold
    , UnParkedCall
    , UserEvent
    , VarSet
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

  enum ChanState{
    ChanStateDown = 0,
    ChanStateReserved,
    ChanStateOffhook,
    ChanStateDialing,
    ChanStateRing,
    ChanStateRinging,
    ChanStateUp,
    ChanStateBusy,
    ChanStateDialingOffhook,
    ChanStatePrering,
    ChanStateUnknown
  };

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
  QString actionExtensionState(const QString &exten, const QString &context=QString());
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
  QString actionPlayDTMF(QString channel, QString digit, int duration=0);
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
  QString actionVoicemailRefresh(QString mailbox, QString context);
  QString actionVoicemailUsersList();
  QString actionWaitEvent();
  QString sendAction(QString action, QVariantMap headers = QVariantMap(), QString presetActionId = QString());
  void insertNotEmpty(QVariantMap *headers, QString key, QVariant value);
protected:

private:
  QMetaEnum responseEnum, eventEnum;
  QVariantMap packetBuffer;
  QString valueToString(QVariant value);
  QVariant stringValue(QString string);

  void dispatchPacket();
  //QString sendAction(QString action, QVariantMap headers = QVariantMap(), QString presetActionId = QString());

private slots:
  void onReadyRead();

signals:
  void connected(QString version);
  void responseSent(AsteriskManager::Response response, QVariantMap headers, QString actionID);
  void eventGenerated(AsteriskManager::Event event, QVariantMap headers);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(AsteriskManager::ExtStatuses)

#endif // ASTERISKMANAGER_H
