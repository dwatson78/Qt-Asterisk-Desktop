#ifndef ASTDIALOGSHOWEVENTS_H
#define ASTDIALOGSHOWEVENTS_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class AstDialogShowEvents;
}

class AstDialogShowEvents : public QDialog
{
    Q_OBJECT

public:
    explicit AstDialogShowEvents(QWidget *parent = 0);
    ~AstDialogShowEvents();
    void loadEvents(QList<QVariantMap> events);

public slots:
    void eventTreecurrentItemChanged(QTreeWidgetItem* current,QTreeWidgetItem* previous);

private:
    Ui::AstDialogShowEvents *ui;
    QMap<QTreeWidgetItem*, QVariantMap> _eventMap;
};

#endif // ASTDIALOGSHOWEVENTS_H
