#include "admxmppbuddyitem.h"

AdmXmppBuddyItem::AdmXmppBuddyItem(QListWidget *parent, const gloox::JID &jid) :
  QObject(parent),
  QListWidgetItem()
{
  _buddy = new AdmXmppBuddyWidget(parent);
  connect(_buddy, SIGNAL(destroying(AdmXmppBuddyWidget*)),
          this,   SLOT(sDestroyingBuddy()));
  _buddy->setName(QString::fromUtf8(jid.username().data()));
  parent->insertItem(parent->count(), this);
  parent->setItemWidget(this,_buddy);
  setSizeHint(_buddy->sizeHint());
}

AdmXmppBuddyWidget* AdmXmppBuddyItem::getBuddyWidget()
{
  return _buddy;
}

AdmXmppBuddyItem::~AdmXmppBuddyItem()
{
  if(NULL != _buddy)
    delete _buddy;
}

void AdmXmppBuddyItem::sDestroyingBuddy()
{
  _buddy = NULL;
}
