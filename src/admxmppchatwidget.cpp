#include "admxmppchatwidget.h"
#include "ui_admxmppchatwidget.h"

#include <QDebug>

AdmXmppChatWidget::AdmXmppChatWidget(MessageSession *session, QWidget *parent) :
  QFrame(parent),
  //MessageFilter(session),
  ui(new Ui::AdmXmppChatWidget)
{
  ui->setupUi(this);
  _session = NULL;
  _chatStateFilter = NULL;
  _messageEventFilter = NULL;
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
    _chatStateFilter->removeChatStateHandler();

  delete ui;
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

      qDebug() << keyEvent->key();
      if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
      {
        qDebug() << "Return pressed";
        if(keyEvent->modifiers().testFlag(Qt::ShiftModifier))
        {
          qDebug() << "Shift was also pressed!";
          if(ui->_newChat->toPlainText().trimmed().isEmpty())
          {
            return true; //Consume the event
          }
        } else {
          qDebug() << ui->_newChat->toPlainText();
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
  qDebug() << QString::fromStdString(session->threadID())
           << QString::fromStdString(msg.from().username())
           << QString::fromStdString(msg.body());
  ui->_chatHistory->addItem(QString("<html><span style='color:blue'>%1</span><span>%2</span></html>")
                            .arg(QString::fromUtf8(msg.from().username().data()))
                            .arg(QString::fromUtf8(msg.body().data()))
  );
  qDebug() << session;
  qDebug() << _session;
  session->send("This is a test message");
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

/*void AdmXmppChatWidget::attachTo(MessageSession *session)
{
  qDebug() << "AdmXmppChatWidget::attachTo";
  qDebug() << QString::fromUtf8(session->threadID().data());
  MessageFilter::attachTo(session);
}

void AdmXmppChatWidget::decorate(Message &msg)
{
  qDebug() << "AdmXmppChatWidget::decorate";
  qDebug() << QString::fromUtf8(msg.body().data());
}

void AdmXmppChatWidget::filter(Message &msg)
{
  qDebug() << "AdmXmppChatWidget::filter";
  qDebug() << QString::fromUtf8(msg.body().data());
}
*/
