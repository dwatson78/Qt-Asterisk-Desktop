#include "admxmppchatblockwidget.h"
#include "ui_admxmppchatblockwidget.h"

#include <QTextDocument>

AdmXmppChatBlockWidget::AdmXmppChatBlockWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmXmppChatBlockWidget)
{
  ui->setupUi(this);
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
  QString msg = Qt::escape(text);
  msg = msg.replace("\n","<br>");
  msg = msg.replace(" ","&nbsp;");
  msg = msg.replace("\t","&nbsp;&nbsp;&nbsp;&nbsp;");
  ui->_chatText->setText(QString("<p>%1</p>").arg(msg));
}
void AdmXmppChatBlockWidget::appendText(const QString &text)
{
  ui->_chatText->setText(ui->_chatText->text().append(Qt::escape(QString("\n\n%1").arg(text))));
  // TODO: Update the time label too
}
