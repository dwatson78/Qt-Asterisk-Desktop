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
  void setName(const QString &name);
  const QString getName();
  const JID& getJid();

public slots:
  void sCustomContextMenu(const QPoint &pos);

  void sSetBuddyName();
  void sRemoveBuddy();
protected:

signals:
  void destroying(AdmXmppBuddyWidget *);
  void sigSetBuddyName(AdmXmppBuddyWidget *, const QString &);
  void sigRemoveBuddy(AdmXmppBuddyWidget *);
  void sigBuddyNameChanged(AdmXmppBuddyWidget *, const QString &);

private:
  Ui::AdmXmppBuddyWidget *ui;
  JID _jid;
};

#endif // ADMXMPPBUDDYWIDGET_H
