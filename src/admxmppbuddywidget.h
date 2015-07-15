#ifndef ADMXMPPBUDDYWIDGET_H
#define ADMXMPPBUDDYWIDGET_H

#include <QFrame>
#include <QListWidgetItem>
#include <gloox/rosteritem.h>
#include <gloox/presence.h>

namespace Ui {
class AdmXmppBuddyWidget;
}
using namespace gloox;
class AdmXmppBuddyWidget : public QFrame
{
  Q_OBJECT
  
public:
  explicit AdmXmppBuddyWidget(QWidget *parent = 0);
  ~AdmXmppBuddyWidget();
  
  void setPresence(const RosterItem &item, const QString &resource, Presence::PresenceType presence, const QString &msg);
  const JID& getJid();
public slots:
  void sChatHistCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
protected:

signals:
  void destroying(AdmXmppBuddyWidget *);

private:
  Ui::AdmXmppBuddyWidget *ui;
  RosterItem *_rosterItem;
  JID _jid;
};

#endif // ADMXMPPBUDDYWIDGET_H
