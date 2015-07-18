#include "admxmppbuddywidget.h"
#include "ui_admxmppbuddywidget.h"

AdmXmppBuddyWidget::AdmXmppBuddyWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::AdmXmppBuddyWidget)
{
  ui->setupUi(this);
}

AdmXmppBuddyWidget::~AdmXmppBuddyWidget()
{
  delete ui;
}

void AdmXmppBuddyWidget::setPresence(const RosterItem &item, const QString &resource, Presence::PresenceType presence, const QString &msg)
{
  Q_UNUSED(resource)
  ui->_buddyName->setText(QString::fromUtf8(item.name().data()));
  ui->_buddyStatusMsg->setText(msg);
  _jid = item.jidJID();
  if(item.online())
  {
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
    ui->_buddyStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/status-offline.png")));
    ui->_buddyStatusMsg->setText("Offline");
  }
}
const JID& AdmXmppBuddyWidget::getJid()
{
  return _jid;
}
