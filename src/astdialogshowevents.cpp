#include "astdialogshowevents.h"
#include "ui_astdialogshowevents.h"

AstDialogShowEvents::AstDialogShowEvents(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AstDialogShowEvents)
{
    ui->setupUi(this);

    ui->_eventTable->setColumnCount(2);
    QStringList headers;
    headers.append("Key");
    headers.append("Value");
    ui->_eventTable->setHorizontalHeaderLabels(headers);

    connect(ui->_eventsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(eventTreecurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

AstDialogShowEvents::~AstDialogShowEvents()
{
    delete ui;
}

void AstDialogShowEvents::loadEvents(QList<QVariantMap> events)
{
    QListIterator<QVariantMap> i(events);
    while(i.hasNext())
    {
        QVariantMap event = i.next();
        QString eventType = event.value("Event").toString();
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, eventType);
        this->ui->_eventsTree->addTopLevelItem(item);
        this->_eventMap.insert(item,event);
    }
}

void AstDialogShowEvents::eventTreecurrentItemChanged(QTreeWidgetItem* current,QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);

    QVariantMap event = _eventMap.value(current);
    this->ui->_eventTable->clearContents();
    this->ui->_eventTable->setRowCount(0);
    int row = 0;
    this->ui->_eventTable->setSortingEnabled(false);
    this->ui->_eventTable->setRowCount(event.size());
    for(QVariantMap::const_iterator iter = event.begin(); iter != event.end(); ++iter)
    {
        QTableWidgetItem *key = new QTableWidgetItem(iter.key());
        QTableWidgetItem *val = new QTableWidgetItem(iter.value().toString());
        ui->_eventTable->setItem(row, 0, key);
        ui->_eventTable->setItem(row, 1, val);
        row++;
    }
    this->ui->_eventTable->resizeColumnsToContents();
    this->ui->_eventTable->setSortingEnabled(true);

}
