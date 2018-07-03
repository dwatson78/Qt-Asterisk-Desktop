#include "admxmppchatwidget.h"
#include "ui_admxmppchatwidget.h"

#include "admstatic.h"

#include <QDebug>

AdmXmppChatWidget::AdmXmppChatWidget(MessageSession *session, QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmXmppChatWidget)
{
  ui->setupUi(this);
  _selfJid = JID();
  _session = NULL;
  _chatStateFilter = NULL;
  _messageEventFilter = NULL;
  _lastChatBlock = NULL;
  _time = NULL;
  _lastChatState = ChatStateInvalid;

  AdmXmppChatWidget::setHeight(ui->_newChat,3);

  ui->_newChat->installEventFilter(this);

  connect(ui->_newChat, SIGNAL(blockCountChanged(int)),
          this,         SLOT(sNewChatBlockCountChanged(int)));
  connect(ui->_newChat->document(), SIGNAL(contentsChanged()),
          this, SLOT(sNewChatContentsChanged()));

  if(NULL != session)
  {
    setMessageSession(session);
  }
}

AdmXmppChatWidget::~AdmXmppChatWidget()
{
  if(_session)
  {
    if(_chatStateFilter)
    {
      setChatState(ChatStateGone);
      _chatStateFilter->removeChatStateHandler();
    }
    if(_messageEventFilter)
      _messageEventFilter->removeMessageEventHandler();

    _session->removeMessageHandler();
  }

  if(_time)
  {
    disconnect(AdmStatic::getInstance()->getTimer(),  SIGNAL(timeout()),
                this,                                   SLOT(sTickTock()));
    delete _time;
    _time = NULL;
  }

  emit destroying(this);
  delete ui;
}

void AdmXmppChatWidget::setSelfJid(JID jid)
{
  _selfJid = jid;
}

MessageSession * AdmXmppChatWidget::messageSession()
{
  return _session;
}

void AdmXmppChatWidget::setMessageSession(MessageSession *session)
{
  _session = session;
  _session->registerMessageHandler(this);

  _messageEventFilter = new MessageEventFilter(_session);
  _messageEventFilter->registerMessageEventHandler(this);

  _chatStateFilter = new ChatStateFilter(_session);
  _chatStateFilter->registerChatStateHandler(this);
  setChatState(ChatStateActive);
}

void AdmXmppChatWidget::setHeight (QPlainTextEdit *edit, int nRows)
{
  QFontMetrics m (edit->font());
  int height = m.lineSpacing();
  edit->setFixedHeight((nRows+1) * height);
}

void AdmXmppChatWidget::sNewChatBlockCountChanged(int)
{
  int bc = ui->_newChat->blockCount();
  int rows = 0;
  if(bc < 3)
    rows = 3;
  else if(bc > 5)
    rows = 5;
  else
    rows = bc;

  AdmXmppChatWidget::setHeight(ui->_newChat,rows);
}
bool AdmXmppChatWidget::eventFilter(QObject *obj, QEvent *event)
{
  if(event->type() == QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent)
    {
      if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
      {
        if(keyEvent->modifiers().testFlag(Qt::ShiftModifier))
        {
          if(ui->_newChat->toPlainText().trimmed().isEmpty())
          {
            return true; //Consume the event
          }
        } else {
          if(!ui->_newChat->toPlainText().trimmed().isEmpty())
          {
            std::string body = ui->_newChat->toPlainText().toUtf8().data();
            _session->send(body);
            setChatStateComposing(false, true);
            Message msg(Message::Chat,_session->target(),body);
            addMsg(_selfJid, msg);
          }
          ui->_newChat->document()->blockSignals(true);
          ui->_newChat->clear();
          ui->_newChat->document()->blockSignals(false);
          return true; //Consume the event
        }
      }
    }
  }
  return QObject::eventFilter(obj, event);
}

void AdmXmppChatWidget::sNewChatContentsChanged()
{
  setChatStateComposing(!ui->_newChat->document()->isEmpty());
}

void AdmXmppChatWidget::handleMessage(const Message &msg, MessageSession *session)
{
  Q_UNUSED(session)

  if(msg.body().empty())
    return;

  addMsg(msg.from(),msg);
}

void AdmXmppChatWidget::handleMessageEvent(const JID &from, MessageEventType event)
{
  qWarning() << "AdmXmppChatWidget::handleMessageEvent: UNHANDLED: "
           << QString::fromUtf8(from.username().data())
           << event;
}

void AdmXmppChatWidget::handleChatState(const JID &from, ChatStateType state)
{
  Q_UNUSED(from)

  switch(state)
  {
    case ChatStateComposing:
      ui->_buddyMsgStatus->setPixmap(QPixmap(":/icons/message-typing.png"));
      break;
    case ChatStatePaused:
      ui->_buddyMsgStatus->setPixmap(QPixmap(":/icons/message-typed.png"));
      break;
    default:
      ui->_buddyMsgStatus->setPixmap(QPixmap());
      break;
  }
}

void AdmXmppChatWidget::addChatBlock(const JID & jid, const Message &msg)
{
  QDateTime time;
  if(msg.when())
  {
    QString timeStamp = QString::fromUtf8(msg.when()->stamp().data());
    time = QDateTime::fromString(timeStamp,Qt::ISODate);
    if(!time.isValid())
      time = QDateTime::currentDateTime();
  } else {
    time = QDateTime::currentDateTime();
  }
  QString spanColor;
  if(jid == _selfJid)
    spanColor = "blue";
  else
    spanColor = "red";
    
  QString msgBody = QString::fromUtf8(msg.body().data()).toHtmlEscaped();
  msgBody = msgBody.replace('\n',"<br/>");
  msgBody = msgBody.replace(' ',"&nbsp;");
  msgBody = msgBody.replace('\t',"&emsp;");

  ui->_chatHistory->append(QString("<span style='color:%1;'>(%2) <b>%3</b></span>: %4")
                               .arg(spanColor)
                               .arg(time.toString("ddd MM/dd hh:mm AP"))
                               .arg(QString::fromUtf8(jid.username().data()))
                               .arg(msgBody));
}

void AdmXmppChatWidget::addMsg(const JID &jid, const Message &msg)
{
  bool isActive = NULL != QApplication::activeWindow();
  if(jid != _selfJid)
  {
    if(!isActive)
    {
      QApplication::alert(window());
      QString from = QString::fromUtf8(jid.username().data()).toHtmlEscaped();
      QString notifBody = QString::fromUtf8(msg.body().data());
      if(notifBody.length() > 80)
        notifBody = notifBody.left(77).append("...");
      notifBody = notifBody.replace('"',"\\\"");
      notifBody = notifBody.toHtmlEscaped();
      notifBody = notifBody.replace(QChar((uint)169),"&copy;");
      
      AdmNotificationManager::showMsg(from, notifBody, "mail-message-new", 7500, this);
    }
    if(isHidden() || !isVisibleTo(qobject_cast<QWidget*>(parent())))
    {
      emit attention(this);
    }
  }
  addChatBlock(jid,msg);
}

void AdmXmppChatWidget::focusChatText()
{
   ui->_newChat->setFocus();
}

void AdmXmppChatWidget::setChatStateComposing(bool composing, bool afterSending)
{
  if(NULL == _chatStateFilter)
    return;

  if(composing)
  {
    if(NULL != _time)
    {
      _time->restart();
    } else {
      setChatState(ChatStateComposing);

      _time = new QTime();
      _time->start();
      connect(AdmStatic::getInstance()->getTimer(), SIGNAL(timeout()),
              this,                                 SLOT(sTickTock()));
    }
  } else {
    if(!afterSending)
    {
      ChatStateType chatState = ui->_newChat->toPlainText().isEmpty() ? ChatStateActive : ChatStatePaused;
      setChatState(chatState);
    } else {
      setChatState(ChatStateActive);
    }
    if(NULL != _time)
    {
      disconnect(AdmStatic::getInstance()->getTimer(),  SIGNAL(timeout()),
                  this,                                   SLOT(sTickTock()));
      delete _time;
      _time = NULL;
    }
  }
}

void AdmXmppChatWidget::setChatState(ChatStateType chatState)
{
  if(chatState != _lastChatState)
  {
    _chatStateFilter->setChatState(chatState);
    _lastChatState = chatState;
  }
}

void AdmXmppChatWidget::sTickTock()
{
  if(NULL == _chatStateFilter)
    return;
    
  if(NULL != _time)
  {
    if(_time->elapsed() >= 6500) //6.5 seconds
    {
      // Set the chat state
      ChatStateType chatState = ui->_newChat->toPlainText().isEmpty() ? ChatStateActive : ChatStatePaused;
      setChatState(chatState);
      
      disconnect(AdmStatic::getInstance()->getTimer(),  SIGNAL(timeout()),
                this,                                   SLOT(sTickTock()));
      delete _time;
      _time = NULL;
    }
  }
}
