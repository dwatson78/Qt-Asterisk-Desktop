#include "admxmppchatwidget.h"
#include "ui_admxmppchatwidget.h"

#include "admxmppchatblockwidget.h"

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
            std::string msg = ui->_newChat->toPlainText().toStdString();
            _session->send(msg);
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
  
  AdmXmppChatBlockWidget *w = new AdmXmppChatBlockWidget(ui->_chatHistory);
  w->setName(QString::fromUtf8(msg.from().username().data()));
  w->setText(QString::fromUtf8(msg.body().data()));
  // TODO: get the delayed delivery time if it exists!!
  QDateTime time = QDateTime::currentDateTime();
  w->setTime(time);
  QListWidgetItem * item = new QListWidgetItem(ui->_chatHistory);
  
  ui->_chatHistory->addItem(item);
  //ui->_chatHistory->setItemWidget(item, lbl);
  ui->_chatHistory->setItemWidget(item, w);
  item->setSizeHint(w->sizeHint());
                            
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
