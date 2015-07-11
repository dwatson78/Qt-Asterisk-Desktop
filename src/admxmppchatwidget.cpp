#include "admxmppchatwidget.h"
#include "ui_admxmppchatwidget.h"

#include <QDebug>

AdmXmppChatWidget::AdmXmppChatWidget(MessageSession *session, QWidget *parent) :
  QFrame(parent),
  //MessageFilter(session),
  ui(new Ui::AdmXmppChatWidget)
{
  ui->setupUi(this);
  _selfJid = JID();
  _session = NULL;
  _chatStateFilter = NULL;
  _messageEventFilter = NULL;
  _lastChatBlock = NULL;
  AdmXmppChatWidget::setHeight(ui->_newChat,3);
  ui->_newChat->installEventFilter(this);
  connect(ui->_newChat, SIGNAL(blockCountChanged(int)),
          this,         SLOT(sNewChatBlockCountChanged(int)));
  if(NULL != session)
  {
    setMessageSession(session);
  }
}

AdmXmppChatWidget::~AdmXmppChatWidget()
{
  emit destroying(this);
  if(_session)
  {
    _session->removeMessageHandler();
    //_session->removeMessageFilter(this);
  }

  if(_messageEventFilter)
    _messageEventFilter->removeMessageEventHandler();

  if(_chatStateFilter)
  {
    _chatStateFilter->setChatState(ChatStateGone);
    _chatStateFilter->removeChatStateHandler();
  }

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

  //Register the message filter
  //attachTo(_session);

  _messageEventFilter = new MessageEventFilter(_session);
  _messageEventFilter->registerMessageEventHandler(this);

  _chatStateFilter = new ChatStateFilter(_session);
  _chatStateFilter->registerChatStateHandler(this);
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
            Message msg(Message::Chat,_session->target(),body);
            addMsg(_selfJid, msg);
          }
          ui->_newChat->clear();
          return true; //Consume the event
        }
      }
    }
  }
  return QObject::eventFilter(obj, event);
}

void AdmXmppChatWidget::handleMessage(const Message &msg, MessageSession *session)
{
  if(msg.body().empty())
    return;
  qDebug() << QString::fromUtf8(session->threadID().data())
           << QString::fromUtf8(msg.from().username().data())
           << QString::fromUtf8(msg.body().data());
  
  addMsg(msg.from(),msg);
                            
  qDebug() << session;
  qDebug() << _session;
}

void AdmXmppChatWidget::handleMessageEvent(const JID &from, MessageEventType event)
{
  qDebug() << "AdmXmppChatWidget::handleMessageEvent";
  qDebug() << QString::fromUtf8(from.username().data()) << event;
}

void AdmXmppChatWidget::handleChatState(const JID &from, ChatStateType state)
{
  qDebug() << "AdmXmppChatWidget::handleChatState";
  qDebug() << QString::fromUtf8(from.username().data()) << state;
}

void AdmXmppChatWidget::addChatBlock(const JID & jid, const Message &msg)
{
  /*AdmXmppChatBlockWidget *w = new AdmXmppChatBlockWidget(ui->_chatHistory);

  w->setName(QString::fromUtf8(jid.username().data()));
  w->setText(QString::fromUtf8(msg.body().data()));
  QDateTime time;
  if(msg.when())
  {
    QString timeStamp = QString::fromUtf8(msg.when()->stamp().data());
    qDebug() << timeStamp;
    w->setEnabled(false);
    time = QDateTime::fromString(timeStamp,Qt::ISODate);
    if(!time.isValid())
      time = QDateTime::currentDateTime();
  } else {
    time = QDateTime::currentDateTime();
  }
  w->setTime(time);
  QListWidgetItem * item = new QListWidgetItem(ui->_chatHistory);
  _lastChatBlock = item;

  ui->_chatHistory->addItem(item);
  ui->_chatHistory->setItemWidget(item, w);
  item->setSizeHint(w->sizeHint());*/
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
  ui->_chatHistory->appendHtml(QString("<span style='color:%1;'>(%2) <b>%3</b></span>: %4")
                               .arg(spanColor)
                               .arg(time.toString("ddd MM/dd hh:mm AP"))
                               .arg(QString::fromUtf8(jid.username().data()))
                               .arg(QString::fromUtf8(msg.body().data())));

  qDebug() << "ActiveWindow: " << QApplication::activeWindow();
}

void AdmXmppChatWidget::addMsg(const JID &jid, const Message &msg)
{
  addChatBlock(jid,msg);
  /*
  if(NULL == _lastChatBlock || ui->_chatHistory->count() < 1)
  {
    _lastJid = jid;
    addChatBlock(jid, msg);
  } else if(_lastJid == jid) {
    QWidget *w = ui->_chatHistory->itemWidget(_lastChatBlock);
    AdmXmppChatBlockWidget *b = qobject_cast<AdmXmppChatBlockWidget*>(w);
    if(b)
    {
      b->appendText(QString::fromUtf8(msg.body().data()));
      _lastChatBlock->setSizeHint(b->sizeHint());
    }
  } else {
    _lastJid = jid;
    addChatBlock(jid, msg);
  }
  ui->_chatHistory->scrollToBottom();

  if(jid != _selfJid && NULL == QApplication::activeWindow())
  {
    QApplication::alert(window());
  }

  if(jid != _selfJid && (isHidden() || !isVisibleTo(qobject_cast<QWidget*>(parent()))))
  {
    qDebug() << "We need to alert the user!!";
    emit attention(this);
  }*/
}
