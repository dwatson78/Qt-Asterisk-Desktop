#ifndef ADMXMPPBUDDYITEM_H
#define ADMXMPPBUDDYITEM_H

#include <QListWidgetItem>
#include <QListWidget>
#include <gloox/jid.h>
#include "admxmppbuddywidget.h"

class AdmXmppBuddyItem : public QObject, public QListWidgetItem
{
  Q_OBJECT
public:
  explicit AdmXmppBuddyItem(QListWidget *parent, const gloox::JID &jid);
  ~AdmXmppBuddyItem();
  AdmXmppBuddyWidget* getBuddyWidget();

  bool operator <(const QListWidgetItem &other) const
  {
    return _buddy->getName().toLower() < ((AdmXmppBuddyItem&)other).getBuddyWidget()->getName().toLower();
  }
  bool operator <=(const QListWidgetItem &other) const
  {
    return _buddy->getName().toLower() <= ((AdmXmppBuddyItem&)other).getBuddyWidget()->getName().toLower();
  }
  bool operator >(const QListWidgetItem &other) const
  {
    return _buddy->getName().toLower() > ((AdmXmppBuddyItem&)other).getBuddyWidget()->getName().toLower();
  }
  bool operator >=(const QListWidgetItem &other) const
  {
    return _buddy->getName().toLower() >= ((AdmXmppBuddyItem&)other).getBuddyWidget()->getName().toLower();
  }
  bool operator ==(const QListWidgetItem &other) const
  {
    return _buddy->getName().toLower() == ((AdmXmppBuddyItem&)other).getBuddyWidget()->getName().toLower();
  }
  bool operator !=(const QListWidgetItem &other) const
  {
    return _buddy->getName().toLower() != ((AdmXmppBuddyItem&)other).getBuddyWidget()->getName().toLower();
  }

public slots:
  void sDestroyingBuddy();
protected:

signals:

private:
  AdmXmppBuddyWidget *_buddy;

};

#endif // ADMXMPPBUDDYITEM_H
