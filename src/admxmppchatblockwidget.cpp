#include "admxmppchatblockwidget.h"
#include "ui_admxmppchatblockwidget.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QStringBuilder>

#include <QDebug>

AdmXmppChatBlockWidget::AdmXmppChatBlockWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmXmppChatBlockWidget)
{
  ui->setupUi(this);

  //QFontMetrics fm(ui->_chatText->fontMetrics());
  //int newWidth = fm.width("mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm");

  //ui->_chatText->setMaximumWidth(newWidth);

  //ui->_chatText->setFixedWidth(newWidth);
  connect(ui->_chatText->document()->documentLayout(),
                SIGNAL(documentSizeChanged(QSizeF)),
          this, SLOT(sDocumentSizeChanged(QSizeF)));
}

AdmXmppChatBlockWidget::~AdmXmppChatBlockWidget()
{
  emit destroying(this);
  delete ui;
}

void AdmXmppChatBlockWidget::setName(const QString &name)
{
  ui->_lblName->setText(name);
}
void AdmXmppChatBlockWidget::setTime(const QDateTime &time)
{
  ui->_lblTime->setText(time.toString("ddd MM/dd hh:mm AP"));
}
void AdmXmppChatBlockWidget::setText(const QString &text)
{
  _msgBody = QString(text);
  ui->_chatText->setPlainText(_msgBody);
  resizeChatText();
  /*QString myText(text);
  int lineWidth = 0;
  for(int i = 0; i < myText.length(); ++i)
  {
    if(myText.at(i) == '\n')
      lineWidth = 0;
    else
      lineWidth += 1;
    if(lineWidth == 80)
    {
      myText.insert(i,QChar(QChar::LineSeparator));
      lineWidth = 0;
    }
  }
  QString msg = Qt::escape(myText);
  msg = msg.replace("\n","<br>");
  msg = msg.replace(" ","&nbsp;");
  msg = msg.replace("\t","&nbsp;&nbsp;&nbsp;&nbsp;");
  ui->_chatText->setText(QString("<p>%1</p>").arg(msg));
  ui->_chatText->setWordWrap(true);*/

}

//QString& AdmXmppChatBlockWidget::doWrap(const QString &text)
//{

 /* QByteArray strBuffer;
  QByteArray wordBuffer;
  int maxCharsPerLine = 80;
  int linePosition = 0;
  int lastWordStart = 0;

  for(int i = 0; i < myText.length(); ++i)
  {
    if(text.at(i) == QChar::LineSeparator)
    {
      linePosition = 0;
    } else if

  };*/

  /*for(int i = 0; i < myText.length(); ++i)
  {
    if(myText.at(i) == '\n')
    {
      lineWidth = 0;
    }
    else if(myText.at(i) == ' '
            || myText.at(i) == '-'
            || myText.at(i) == '-' )
    {
      lastWardStart = i;
    }
    else
    {
      lineWidth += 1;
    }
    if(lineWidth == 80)
    {
      myText.insert(i,QChar(QChar::LineSeparator));
      lineWidth = 0;
    }*/

//}

void AdmXmppChatBlockWidget::appendText(const QString &text)
{
  _msgBody = _msgBody.append("\n\n").append(text);
  /*QString myText(_msgBody);
  QString msg = Qt::escape(myText);
  msg = msg.replace("\n","<br>");
  msg = msg.replace(" ","&nbsp;");
  msg = msg.replace("\t","&nbsp;&nbsp;&nbsp;&nbsp;");*/
  ui->_chatText->setPlainText(_msgBody);
  ui->_lblTime->setText(QDateTime::currentDateTime().toString("ddd MM/dd hh:mm AP"));
  resizeChatText();
  /*qDebug() << "sizeHint: " << sizeHint().width() << sizeHint().height();
  qDebug() << "sizeHint _lblName: " << ui->_lblName->sizeHint().width() << ui->_lblName->sizeHint().height();
  qDebug() << "sizeHint _chatText: " << ui->_chatText->sizeHint().width() << ui->_chatText->sizeHint().height();
  qDebug() << "sizeHint _lblTime: " << ui->_lblTime->sizeHint().width() << ui->_lblTime->sizeHint().height();*/

  //Fix size bug
  if(sizeHint().height() < (ui->_lblName->sizeHint().height()
                            + ui->_chatText->sizeHint().height()
                            + ui->_lblTime->sizeHint().height()))
  {
    setMinimumHeight(
          6 //3 elements with 2 pixels between each = 6 pixels
        + ui->_lblName->sizeHint().height()
        + ui->_chatText->sizeHint().height()
        + ui->_lblTime->sizeHint().height());
  }
}
void AdmXmppChatBlockWidget::resizeChatText()
{
  /*int minHeight = 20;
  int maxHeight = 200;
  int viewHeight = ui->_chatText->viewport()->size().height();
  int setHeight = minHeight;
  if(minHeight < viewHeight && viewHeight < maxHeight)
    setHeight = viewHeight;
  else if(viewHeight > maxHeight)
    setHeight = maxHeight;
  ui->_chatText->setMinimumHeight(setHeight);
  ui->_chatText->setMaximumHeight(setHeight);*/
}

void AdmXmppChatBlockWidget::sDocumentSizeChanged(const QSizeF &size)
{
  qDebug() << size.width() << size.height();
  qDebug() << ui->_chatText->viewport()->size().width()
           << ui->_chatText->viewport()->size().height();
  qDebug() << ui->_chatText->viewport()->sizeHint().width()
           << ui->_chatText->viewport()->sizeHint().height();

  //ui->_chatText->setMinimumHeight(ui->_chatText->viewport()->size().height());
  //ui->_chatText->setMaximumHeight(ui->_chatText->viewport()->size().height());
}
