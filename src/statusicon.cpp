#include "statusicon.h"

#include <QDebug>

StatusIcon::StatusIcon(QWidget *parent):
  QWidget(parent)
{
  isConnected = false;
  isLoggedIn = false; 
  _lblText = new QLabel("Disconnected:");
  _lblIcon = new QLabel();
  _lblIcon->setPixmap(QPixmap(":/icons/dialog-close.png"));
  _lyt = new QHBoxLayout(this);
  _lyt->setMargin(0);
  _lyt->addWidget(_lblText);
  _lyt->addWidget(_lblIcon);
}

void StatusIcon::setConnected(bool connected)
{
  isConnected = connected;
  _lblText->setText(connected ? "Connected:" : "Disconnected:");
  _lblIcon->setPixmap(QPixmap(connected? ":/icons/dialog-ok-apply.png" : ":/icons/dialog-close.png"));

}

void StatusIcon::setLoggedIn(bool loggedIn, QString message)
{
  isLoggedIn = loggedIn;
  _lblText->setText(loggedIn ? "Logged In:" : tr("Login Failure (%1):").arg(message));
  _lblIcon->setPixmap(QPixmap(loggedIn? ":/icons/dialog-ok-apply.png" : ":/icons/dialog-close.png"));
}
