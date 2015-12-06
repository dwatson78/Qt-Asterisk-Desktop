#include "admxmppbuddywidget.h"
#include "ui_admxmppbuddywidget.h"

#include <QMenu>
#include <QMenuItem>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>

AdmXmppBuddyWidget::AdmXmppBuddyWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmXmppBuddyWidget)
{
  ui->setupUi(this);

  ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-offline.png")));
  ui->_buddyStatusMsg->setText("Offline");

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this,SIGNAL(customContextMenuRequested(QPoint)),
          this,SLOT(sCustomContextMenu(QPoint)));
}

AdmXmppBuddyWidget::~AdmXmppBuddyWidget()
{
  emit destroying(this);
  delete ui;
}

void AdmXmppBuddyWidget::setPresence(const RosterItem &item, const QString &resource, Presence::PresenceType presence, const QString &msg)
{
  Q_UNUSED(resource)
  QString name = QString::fromUtf8(item.name().data());
  if(name.trimmed().isEmpty())
    name = QString::fromUtf8(item.jidJID().username().data());
  setName(name);

  ui->_buddyStatusMsg->setText(msg);
  if(item.online())
  {
    _jid = item.jidJID();
    switch(presence)
    {
      case Presence::Available:
      case Presence::Chat:
        ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-avail.png")));
        if(msg.isNull() || msg.isEmpty())
        {
          if(presence == Presence::Available)
          {
            ui->_buddyStatusMsg->setText("Available");
          } else if(presence == Presence::Chat) {
            ui->_buddyStatusMsg->setText("Chatty");
          }
        }
        break;
      case Presence::Away:
        ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-busy.png")));
        if(msg.isNull() || msg.isEmpty())
        {
          ui->_buddyStatusMsg->setText("Busy");
        }
        break;
      case Presence::XA:
        ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-unavail.png")));
        if(msg.isNull() || msg.isEmpty())
        {
          ui->_buddyStatusMsg->setText("Away");
        }
        break;
      case Presence::DND:
        ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-dnd.png")));
        if(msg.isNull() || msg.isEmpty())
        {
          ui->_buddyStatusMsg->setText("Do Not Disturb");
        }
        break;
      case Presence::Unavailable:
        ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-offline.png")));
        ui->_buddyStatusMsg->setText("Offline");
        break;
      default:
        break;
    }
  } else {
    _jid = JID();
    ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-offline.png")));
    ui->_buddyStatusMsg->setText("Offline");
  }
}

void AdmXmppBuddyWidget::setName(const QString &name)
{
  if(name != ui->_buddyName->text())
  {
    ui->_buddyName->setText(name);
    emit sigBuddyNameChanged(this, name);
  }
}
const QString AdmXmppBuddyWidget::getName()
{
  return ui->_buddyName->text();
}

const JID& AdmXmppBuddyWidget::getJid()
{
  return _jid;
}

void AdmXmppBuddyWidget::sCustomContextMenu(const QPoint &pos)
{
  QMenu m;

  // Set Buddy Name
  QAction setBuddyName("Set Buddy Name...",&m);
  connect(&setBuddyName,SIGNAL(triggered()),
          this,SLOT(sSetBuddyName()));
  m.addAction(&setBuddyName);

  // Remove Buddy
  QAction removeBuddy("Remove Buddy",&m);
  connect(&removeBuddy,SIGNAL(triggered()),
          this,SLOT(sRemoveBuddy()));
  m.addAction(&removeBuddy);

  // Show the menu
  m.exec(mapToGlobal(pos));
}

void AdmXmppBuddyWidget::sSetBuddyName()
{
  bool ok = false;
  QString buddyName = QInputDialog::getText(this,"Set Buddy Name","Buddy name:",QLineEdit::Normal,ui->_buddyName->text(),&ok);
  if(ok && !buddyName.trimmed().isEmpty())
  {
    emit sigSetBuddyName(this, buddyName);
  }
}

void AdmXmppBuddyWidget::sRemoveBuddy()
{
  QString msg = QString("Are you sure you wish to remove '%1' from your list?").arg(ui->_buddyName->text());
  QMessageBox b(QMessageBox::Question,"Remove Buddy?",msg,QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,this);
  int result = b.exec();
  if(result == QMessageBox::Yes)
  {
    emit sigRemoveBuddy(this);
    deleteLater();
  }
}
