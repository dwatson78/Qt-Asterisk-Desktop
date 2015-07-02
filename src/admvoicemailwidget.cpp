#include "admvoicemailwidget.h"
#include "ui_admvoicemailwidget.h"
#include "restapiastvm.h"

#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QBuffer>
#include <QFile>
#include <QSound>

AdmVoiceMailWidget::AdmVoiceMailWidget(QString vmBox, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AdmVoiceMailWidget)
{
  ui->setupUi(this);
  _vmBox = vmBox;

  // Setup the table
  ui->_messages->setColumnCount(1);
  QStringList headers;
  headers.append("Date");
  headers.append("Call-Id");
  ui->_messages->setColumnCount(headers.count());
  ui->_messages->setHorizontalHeaderLabels(headers);
  ui->_messages->setStyleSheet("QTableWidget::item{padding:0px;}");
  connect(ui->_messages,  SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
          this,           SLOT(sMessagesItemChanged(QTableWidgetItem*,QTableWidgetItem*))
  );

  // Get the intial voicemail counts
  RestApiAstVmMsgCounts *rest = new RestApiAstVmMsgCounts();
  QVariantMap params;
  params["vmBox"] = _vmBox;
  bool ok = false;
  rest->set(params, &ok);
  if(ok)
  {
    connect(rest, SIGNAL(sigReady(QVariantMap)),
            this, SLOT(sVmCountReady(QVariantMap)));
    connect(rest, SIGNAL(sigError(QNetworkReply::NetworkError)),
            this, SLOT(sVmCountError(QNetworkReply::NetworkError)));
    rest->start();
  }

  connect(ui->_folders, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
          this,         SLOT(sFolderItemActivated(QListWidgetItem*,QListWidgetItem*)));

  connect(ui->_playMsg, SIGNAL(clicked()),
          this,         SLOT(sPlayMsgClicked()));

}

AdmVoiceMailWidget::~AdmVoiceMailWidget()
{
  delete ui;
}
void AdmVoiceMailWidget::sVmCountReady(const QVariantMap &data)
{
  if(data["result"] == "ok")
  {
    QVariantMap resultMap = data["response"].toMap();
    for(QVariantMap::const_iterator i = resultMap.begin(); i != resultMap.end(); ++i)
    {
      bool hasItem = false;
      for(int i2 = 0; i2 < ui->_folders->count(); ++i2)
      {
        if(ui->_folders->item(i2)->text() == i.key())
        {
          hasItem = true;
          break;
        }
      }
      if(!hasItem)
      {
        if(i.key() == "INBOX")
        {
          ui->_folders->insertItem(0,QString("%1 (%2)").arg("New").arg(i.value().toUInt()));
          QFont f = ui->_folders->item(0)->font();
          f.setBold(true);
          ui->_folders->item(0)->setFont(f);
        }
        else
          ui->_folders->addItem(QString("%1 (%2)").arg(i.key()).arg(i.value().toUInt()));
      }
    }
  }
}
void AdmVoiceMailWidget::sVmCountError(QNetworkReply::NetworkError err)
{
  Q_UNUSED(err)
}

void AdmVoiceMailWidget::sFolderItemActivated(QListWidgetItem *current, QListWidgetItem *previous)
{
  Q_UNUSED(previous)
  ui->_messages->setRowCount(0);
  // Get the message details for this folder

  QString folder = current->text();
  QRegExp re("^(.*)\\ \\([0-9]+\\)");
  if(re.exactMatch(folder))
  {
    QVariantMap params;
    params["vmBox"] = _vmBox;
    params["vmFolder"] = re.cap(1);
    bool ok = false;
    RestApiAstVmMsgDetails *dets = new RestApiAstVmMsgDetails();
    dets->set(params, &ok);
    if(ok)
    {
      connect(dets, SIGNAL(sigReady(QVariantMap)),
              this, SLOT(sVmMsgDetailsReady(QVariantMap)));
      connect(dets, SIGNAL(sigError(QNetworkReply::NetworkError)),
              this, SLOT(sVmMsgDetailsError(QNetworkReply::NetworkError)));
      dets->start();
    }
  }


  //ui->_messages->clear();
  /*ui->_messages->setRowCount(0);
  ui->_messages->insertRow(0);
  QTableWidgetItem *cellitem = new QTableWidgetItem(current->text());
  ui->_messages->setItem(0,0,cellitem);*/
}

void AdmVoiceMailWidget::sVmMsgDetailsReady(const QVariantMap &data)
{
  Q_UNUSED(data)
  qDebug() << data.count();
  if(data["result"] == "ok")
  {
    QList<QVariant> msgs = data["response"].toList();
    if(!msgs.isEmpty())
    {
      int row = -1;
      QVariant msgData;
      QVariantMap msgDataMap;
      QTableWidgetItem *cell = NULL;

      for(QList<QVariant>::const_iterator i = msgs.begin(); i != msgs.end(); ++i)
      {
        msgData = *i;
        msgDataMap = msgData.toMap();
        row++;
        ui->_messages->insertRow(row);

        cell = new QTableWidgetItem(msgDataMap["origdate"].toString());
        ui->_messages->setItem(row,0,cell);
        cell->setData(Qt::UserRole, msgData);
        
        cell = new QTableWidgetItem(msgDataMap["callerid"].toString());
        ui->_messages->setItem(row,1,cell);
      }
    }
  }
}

void AdmVoiceMailWidget::sVmMsgDetailsError(QNetworkReply::NetworkError err)
{
  Q_UNUSED(err)
  qDebug() << err;
}

void AdmVoiceMailWidget::sMessagesItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
  Q_UNUSED(current)
  Q_UNUSED(previous)
  qDebug() << QString("AdmVoiceMailWidget::sMessagesItemChanged: Previous Null: %1").arg(previous == NULL);
  qDebug() << QString("AdmVoiceMailWidget::sMessagesItemChanged: Current Null: %1").arg(current == NULL);
}

void AdmVoiceMailWidget::sPlayMsgClicked()
{

  qDebug() << QString("Selected row: %1").arg(ui->_messages->selectedItems().at(0)->row());
  QVariant data = ui->_messages->item(ui->_messages->selectedItems().at(0)->row(),0)->data(Qt::UserRole);
  QVariantMap dataMap = data.toMap();
  qDebug() << dataMap.value("sndfile");
  RestApiAstVmGetMsgSoundFile *r = new RestApiAstVmGetMsgSoundFile();
  bool ok = false;
  QVariantMap params;
  params["vmBox"] = _vmBox;
  QString folder = ui->_folders->currentItem()->text();
  QRegExp re("^(.*)\\ \\([0-9]+\\)");
  if(re.exactMatch(folder))
  {
    params["vmFolder"] = re.cap(1);
  }
  params["vmFile"] = dataMap.value("sndfile");

  r->set(params, &ok);
  if(ok)
  {
    connect(r,SIGNAL(sigReady(QByteArray)),this,SLOT(sSoundFileReady(QByteArray)));
    r->start();
  }
}

void AdmVoiceMailWidget::sSoundFileReady(const QByteArray &data)
{
  qDebug() << data.length();
  QFile file("/tmp/msg.wav");
  file.open(QIODevice::WriteOnly);
  file.write(data);
  file.close();
  QSound *snd = new QSound("/tmp/msg.wav");
  qDebug() << "Start";
  snd->play();
  qDebug() << "Done";
}
