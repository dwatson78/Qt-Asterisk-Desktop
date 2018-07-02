#include "admvoicemailwidget.h"
#include "ui_admvoicemailwidget.h"
#include "qtasteriskdesktopmain.h"
#include "restapiastvm.h"

#include <QDebug>
#include "admstatic.h"
#include <QDir>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QMessageBox>
#include <QMetaObject>
#include <QMetaEnum>
#include <QInputDialog>

AdmVoiceMailWidget::AdmVoiceMailWidget(QString mailbox, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AdmVoiceMailWidget)
{
  ui->setupUi(this);
  _mailbox = mailbox;
  _vmBox = mailbox;
  _vmContext = "default";
  if(_mailbox.contains("@"))
  {
    QStringList vmBoxParts = _mailbox.split("@");
    if(vmBoxParts.count() == 2)
    {
      _vmBox = vmBoxParts[0];
      _vmContext = vmBoxParts[1];
    }
  }
  
  _mObj = new Phonon::MediaObject();
  _mSrc = Phonon::MediaSource();
  _mOut = new Phonon::AudioOutput(Phonon::CommunicationCategory, this);
  Phonon::createPath(_mObj,_mOut);
  //_mFile = NULL;
  _selectedFolder = QString();
  _selectedMessageRow = -1;

  // Setup the table
  ui->_messages->setWordWrap(false);
  ui->_messages->verticalHeader()->setVisible(false);
  QStringList headers;
  headers.append("Date");
  headers.append("Call-Id");
  ui->_messages->setColumnCount(headers.count());
  ui->_messages->setHorizontalHeaderLabels(headers);
  ui->_messages->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  ui->_messages->horizontalHeader()->setStretchLastSection(true);
  /*ui->_messages->horizontalHeaderItem(0)->setSizeHint(
        QSize(200, ui->_messages->horizontalHeaderItem(0)->sizeHint().height()));*/

  ui->_messages->setStyleSheet("QTableWidget::item{margin: 2px; padding: 2px;} ");
  connect(ui->_messages,  SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
          this,           SLOT(sMessagesItemChanged(QTableWidgetItem*,QTableWidgetItem*))
  );

  connect(ui->_folders, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
          this,         SLOT(sFolderItemActivated(QListWidgetItem*,QListWidgetItem*)));

  ui->_messages->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->_messages,  SIGNAL(customContextMenuRequested(QPoint)),
          this,           SLOT(sMsgCustomContextMenuRequested(QPoint)));

  connect(ui->_msgTogglePlay, SIGNAL(clicked()),
          this,         SLOT(sPlayMsgClicked()));
          
  ui->_seekSlider->setMediaObject(_mObj);
  ui->_volumeSlider->setAudioOutput(_mOut);
  
  connect(_mObj,  SIGNAL(tick(qint64)),
          this,   SLOT(sMediaObjectTick(qint64)));
  connect(_mObj,  SIGNAL(stateChanged(Phonon::State,Phonon::State)),
          this,   SLOT(sMediaObjectStateChanged(Phonon::State,Phonon::State)));
  connect(_mObj,  SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sMediaObjectSourceChanged(Phonon::MediaSource)));
  connect(_mObj,  SIGNAL(finished()),
          this,   SLOT(sMediaObjectFinished()));
  connect(ui->_msgSeekBack, SIGNAL(clicked()),
          this,             SLOT(sMsgSeekBackClick()));
  connect(ui->_msgSeekForward,  SIGNAL(clicked()),
          this,                 SLOT(sMsgSeekForwardClick()));
  connect(ui->_playOnPhone, SIGNAL(clicked()),
          this,             SLOT(sPlayOnPhoneClicked()));

  ui->_frameSelectedMessage->setEnabled(false);
  ui->_lcdTime->display("00:00");

  // Get the intial voicemail counts
  sRefreshView();
}

AdmVoiceMailWidget::~AdmVoiceMailWidget()
{
  _mObj->clearQueue();
  _mObj->clear();
  removeTmpMsgFiles();

  delete ui;
  //if(_mFile != NULL)
  //  delete _mFile;
  delete _mOut;
  delete _mObj;
}
void AdmVoiceMailWidget::sVmCountReady(const QVariantMap &data)
{
  if(data["result"] == "ok")
  {
    QVariantMap resultMap = data["response"].toMap();
    QListWidgetItem *item = NULL;
    for(QVariantMap::const_iterator i = resultMap.begin(); i != resultMap.end(); ++i)
    {
      bool hasItem = false;
      for(int i2 = 0; i2 < ui->_folders->count(); ++i2)
      {
        if(i.key() == ui->_folders->item(i2)->data(RestApiAstVm::ROLE_FOLDER_NAME).toString())
        {
          hasItem = true;
          item = ui->_folders->item(i2);
          item->setData(RestApiAstVm::ROLE_FOLDER_MSG_COUNT, i.value());
          _setFolderItemName(item);
          break;
        }
      }
      if(!hasItem)
      {
        item = new QListWidgetItem();
        item->setData(RestApiAstVm::ROLE_FOLDER_NAME, i.key());
        item->setData(RestApiAstVm::ROLE_FOLDER_MSG_COUNT, i.value());
        _setFolderItemName(item);
        if(i.key() == "INBOX")
          ui->_folders->insertItem(0,item);
        else
          ui->_folders->addItem(item);
      }
    }
    if(!_selectedFolder.isNull())
    {
      for(int i = 0; i < ui->_folders->count(); ++i)
      {
        if(_selectedFolder == ui->_folders->item(i)->data(RestApiAstVm::ROLE_FOLDER_NAME).toString())
        {
          ui->_folders->setCurrentRow(i);
          sLoadFolder(_selectedFolder);
        }
      }
    }
  }
}

void AdmVoiceMailWidget::_setFolderItemName(QListWidgetItem *item)
{
  QString folderName = item->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  uint folderCount = item->data(RestApiAstVm::ROLE_FOLDER_MSG_COUNT).toUInt();
  if(folderName == "INBOX")
  {
    item->setText(QString("%1 (%2)")
                  .arg("New")
                  .arg(folderCount));
    QFont f = item->font();
    f.setBold(folderCount > 0);
    item->setFont(f);
  } else {
    item->setText(QString("%1 (%2)")
                  .arg(folderName)
                  .arg(folderCount));
  }
}

void AdmVoiceMailWidget::sVmCountError(QNetworkReply::NetworkError err, const QString &errString)
{
  showNetworkErrorMsg("sVmCountError", err, errString);
}

void AdmVoiceMailWidget::sFolderItemActivated(QListWidgetItem *current, QListWidgetItem *previous)
{
  Q_UNUSED(previous)
  QString folder = current->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  sLoadFolder(folder);
}
void AdmVoiceMailWidget::sLoadFolder(const QString &folder)
{
  ui->_messages->setRowCount(0);
  // Get the message details for this folder
  QVariantMap params;
  params["vmContext"] = _vmContext;
  params["vmBox"] = _vmBox;
  params["vmFolder"] = folder;
  bool ok = false;
  RestApiAstVmMsgDetails *dets = new RestApiAstVmMsgDetails();
  dets->set(params, &ok);
  if(ok)
  {
    connect(dets, SIGNAL(sigReady(QVariantMap)),
            this, SLOT(sVmMsgDetailsReady(QVariantMap)));
    connect(dets, SIGNAL(sigError(QNetworkReply::NetworkError, QString)),
            this, SLOT(sVmMsgDetailsError(QNetworkReply::NetworkError, QString)));
    dets->start();
  }
}

void AdmVoiceMailWidget::sVmMsgDetailsReady(const QVariantMap &data)
{
  if(data["result"] == "ok")
  {
    QList<QVariant> msgs = data["response"].toList();
    if(!msgs.isEmpty())
    {
      int row = -1;
      QVariant msgData;
      QVariantMap msgDataMap;
      QTableWidgetItem *cell = NULL;
      ui->_messages->setVisible(false);
      for(QList<QVariant>::const_iterator i = msgs.begin(); i != msgs.end(); ++i)
      {
        msgData = *i;
        msgDataMap = msgData.toMap();
        row++;
        ui->_messages->insertRow(row);

        QDateTime dt = QDateTime::fromString(
              msgDataMap["origdate"].toString(),
              "ddd MMM dd hh:mm:ss A 'UTC' yyyy");
        if(!dt.isValid())
        {
          dt = QDateTime::fromString(
                msgDataMap["origdate"].toString(),
                "ddd MMM  d hh:mm:ss A 'UTC' yyyy");
        }
        if(dt.isValid())
        {
          dt.setTimeSpec(Qt::UTC);
          cell = new QTableWidgetItem(dt.toLocalTime().toString("ddd MM/dd hh:mm:ss AP"));
        } else {
          cell = new QTableWidgetItem(msgDataMap["origdate"].toString());
        }
        cell->setData(Qt::UserRole, msgData);
        cell->setSizeHint(QSize(180,20));
        ui->_messages->setItem(row,0,cell);
        
        cell = new QTableWidgetItem(msgDataMap["callerid"].toString());
        cell->setSizeHint(QSize(200,20));
        ui->_messages->setItem(row,1,cell);
      }
      ui->_messages->resizeColumnsToContents();
      ui->_messages->resizeRowsToContents();
      ui->_messages->setVisible(true);
    }
  }
}

void AdmVoiceMailWidget::sVmMsgDetailsError(QNetworkReply::NetworkError err, const QString &errString)
{
  showNetworkErrorMsg("sVmMsgDetailsError", err, errString);
}

void AdmVoiceMailWidget::sMessagesItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
  Q_UNUSED(previous)

  _mObj->clearQueue();
  _mObj->clear();
  //if(NULL != _mFile)
  //{
    //delete _mFile;
    //_mFile = NULL;
  //}
  ui->_frameSelectedMessage->setEnabled(NULL != current);
}

void AdmVoiceMailWidget::sMsgCustomContextMenuRequested(QPoint pos)
{
  if(ui->_messages->currentRow() < 0)
    return;
  QMenu m;
  QMenu m2;
  m2.setTitle("Move to");
  m.addMenu(&m2);
  QString selectedFolder = ui->_folders->selectedItems().at(0)->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  if(selectedFolder != "INBOX")
    m2.addAction(new QAction("New",this));
  if(selectedFolder != "Old")
    m2.addAction(new QAction("Old",this));
  if(selectedFolder != "Urgent")
    m2.addAction(new QAction("Urgent",this));
  if(selectedFolder != "Family")
    m2.addAction(new QAction("Family",this));
  if(selectedFolder != "Friends")
    m2.addAction(new QAction("Friends",this));
  if(selectedFolder != "Work")
    m2.addAction(new QAction("Work",this));
  m.addAction(new QAction("Forward to...", this));
  m.addSeparator();
  if(selectedFolder != "Deleted")
    m.addAction(new QAction("Archive",this));
  m.addAction(new QAction("Delete",this));

  connect(&m,   SIGNAL(triggered(QAction*)),
          this, SLOT(sMsgCustomContextAction(QAction*)));
  connect(&m2,   SIGNAL(triggered(QAction*)),
          this, SLOT(sMsgCustomContextMoveMsg(QAction*)));
  m.exec(ui->_messages->viewport()->mapToGlobal(pos));
}
void AdmVoiceMailWidget::sMsgCustomContextAction(QAction* action)
{
  QString folder = ui->_folders->currentItem()->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  QVariant data = ui->_messages->item(ui->_messages->currentRow(),0)->data(Qt::UserRole);
  QVariantMap dataMap = data.toMap();
  if(action->text() == "Forward to...")
  {
    bool ok;
    QString destBox = QInputDialog::getText(
          this,
          tr("Forward to mailbox..."),
          tr("Mailbox:"),
          QLineEdit::Normal,
          "",
          &ok
    );
    QRegExp re("^[0-9]+$");
    if(re.exactMatch(destBox))
    {
      QVariantMap params;
      params["vmContext"] = _vmContext;
      params["vmBox"] = _vmBox;
      params["vmFolder"] = folder;
      params["vmMsgId"] = dataMap["msg_id"].toString();
      params["vmDestBox"] = destBox;
      bool ok = false;
      RestApiAstVmForwardMessage *fwd = new RestApiAstVmForwardMessage();
      fwd->set(params, &ok);
      if(ok)
      {
        connect(fwd,  SIGNAL(sigReady(QVariantMap)),
                this, SLOT(sVmForwardMessageReady(QVariantMap))
        );
        connect(fwd,  SIGNAL(sigError(QNetworkReply::NetworkError, QString)),
                this, SLOT(sVmForwardMessageError(QNetworkReply::NetworkError, QString))
        );
        fwd->start();
      }
    }
  } else if(action->text() == "Archive")
  {
    qDebug() << AdmStatic::eventToString(dataMap);

    if(folder != "Deleted")
    {
      QDateTime qdt;
      qdt.setTime_t(dataMap["origtime"].toUInt());

      if(QMessageBox(QMessageBox::Question
                     ,QString("Archive Message?")
                     ,QString("Are you sure you want to archive this message?\n\nFrom: %1\nDate: %2\nOriginal Mailbox: %3")
                     .arg(dataMap["callerid"].toString())
                     .arg(qdt.toLocalTime().toString("ddd MM/dd hh:mm:ss AP"))
                     .arg(dataMap["origmailbox"].toString())
                     ,(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
         .exec() == QMessageBox::Yes)
      {
        QAction *archive = new QAction("Deleted",this);
        sMsgCustomContextMoveMsg(archive);
      }
    }
  } else if (action->text() == "Delete") {
    QDateTime qdt;
    qdt.setTime_t(dataMap["origtime"].toUInt());
    if(QMessageBox(QMessageBox::Question
                   ,QString("Delete Message?")
                   ,QString("Are you sure you want to delete this message?\n\nFrom: %1\nDate: %2\nOriginal Mailbox: %3")
                   .arg(dataMap["callerid"].toString())
                   .arg(qdt.toLocalTime().toString("ddd MM/dd hh:mm:ss AP"))
                   .arg(dataMap["origmailbox"].toString())
                   ,(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
       .exec() == QMessageBox::Yes)
    {
      QVariantMap params;
      params["vmContext"] = _vmContext;
      params["vmBox"] = _vmBox;
      params["vmFolder"] = folder;
      params["vmMsgId"] = dataMap["msg_id"].toString();
      bool ok = false;
      RestApiAstVmDeleteMessage *r = new RestApiAstVmDeleteMessage();
      r->set(params, &ok);
      if(ok)
      {
        connect(r,    SIGNAL(sigReady(QVariantMap)),
                this, SLOT(sVmDeleteMessageReady(QVariantMap)));
        connect(r,    SIGNAL(sigError(QNetworkReply::NetworkError, QString)),
                this, SLOT(sVmDeleteMessageError(QNetworkReply::NetworkError, QString)));
        r->start();
      }
    }
  }
}

void AdmVoiceMailWidget::sVmForwardMessageReady(const QVariantMap &data)
{
  Q_UNUSED(data);
}
void AdmVoiceMailWidget::sVmForwardMessageError(QNetworkReply::NetworkError err, const QString &errString)
{
  showNetworkErrorMsg("sVmForwardMessageError", err, errString);
}

void AdmVoiceMailWidget::sVmDeleteMessageReady(const QVariantMap &data)
{
  Q_UNUSED(data);
  sRefreshView();
  sSendAmiVoicemailRefresh();
}
void AdmVoiceMailWidget::sVmDeleteMessageError(QNetworkReply::NetworkError err, const QString &errString)
{
  showNetworkErrorMsg("sVmDeleteMessageError", err, errString);
}

void AdmVoiceMailWidget::sMsgCustomContextMoveMsg(QAction* action)
{
  if(action->text() != "")
  {
    QVariant data = ui->_messages->item(ui->_messages->currentRow(),0)->data(Qt::UserRole);
    QVariantMap dataMap = data.toMap();

    RestApiAstVmMoveMessage *move = new RestApiAstVmMoveMessage();

    bool ok = false;
    QVariantMap params;
    params["vmContext"] = _vmContext;
    params["vmBox"] = _vmBox;
    params["vmFolderSrc"] = ui->_folders->currentItem()->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
    params["vmFolderDst"] = action->text() == "New" ? "INBOX" : action->text();
    params["vmMsgId"] = dataMap.value("msg_id").toString();
    move->set(params,&ok);
    if(ok)
    {
      connect(move, SIGNAL(sigReady(QVariantMap)),
              this, SLOT(sVmMoveMessageReady(QVariantMap)));
      connect(move, SIGNAL(sigError(QNetworkReply::NetworkError, QString)),
              this, SLOT(sVmMoveMessageError(QNetworkReply::NetworkError, QString)));
      move->start();
    }
  }
}
void AdmVoiceMailWidget::sVmMoveMessageReady(const QVariantMap &data)
{
  Q_UNUSED(data)
  sRefreshView();
  sSendAmiVoicemailRefresh();
  //QString folder = ui->_folders->currentItem()->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  //sLoadFolder(folder);
}
void AdmVoiceMailWidget::sRefreshView()
{
  QListWidgetItem *selectedFolder = ui->_folders->currentItem();
  if(NULL != selectedFolder)
    _selectedFolder = selectedFolder->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  else
    _selectedFolder = QString();

  RestApiAstVmMsgCounts *rest = new RestApiAstVmMsgCounts();
  QVariantMap params;
  params["vmContext"] = _vmContext;
  params["vmBox"] = _vmBox;
  bool ok = false;
  rest->set(params, &ok);
  if(ok)
  {
    connect(rest, SIGNAL(sigReady(QVariantMap)),
            this, SLOT(sVmCountReady(QVariantMap)));
    connect(rest, SIGNAL(sigError(QNetworkReply::NetworkError, QString)),
            this, SLOT(sVmCountError(QNetworkReply::NetworkError, QString)));
    rest->start();
  }
}

void AdmVoiceMailWidget::sVmMoveMessageError(QNetworkReply::NetworkError err, const QString &errString)
{
  showNetworkErrorMsg("sVmMoveMessageError", err, errString);
}

void AdmVoiceMailWidget::sPlayMsgClicked()
{
  if(_mObj->state() == Phonon::PlayingState)
  {
    _mObj->pause();
  } else {
    if(_mObj->currentSource().type() == Phonon::MediaSource::Empty
       || _mObj->currentSource().type() == 5)
    {
      if(ui->_messages->selectedItems().count() > 0)
      {
        QVariant data = ui->_messages->item(ui->_messages->selectedItems().at(0)->row(),0)->data(Qt::UserRole);
        QVariantMap dataMap = data.toMap();
        RestApiAstVmGetMsgSoundFile *r = new RestApiAstVmGetMsgSoundFile();
        bool ok = false;
        QVariantMap params;
        params["vmContext"] = _vmContext;
        params["vmBox"] = _vmBox;
        QString folder = ui->_folders->currentItem()->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
        params["vmFolder"] = folder;
        params["vmMsgId"] = dataMap.value("msg_id").toString();
        r->set(params, &ok);
        if(ok)
        {
          connect(r,SIGNAL(sigReady(QByteArray)),this,SLOT(sSoundFileReady(QByteArray)));
          r->start();
        } else {
          qCritical() << QString("AdmVoiceMailWidget::sPlayMsgClicked: RestApiVm::set(...) indicated failure to set all parameters");
          QMessageBox::critical(this,"Rest API Error", "Failed to set the proper paremeters for the RestApiAstVmGetMsgSoundFile object.");
        }
      }
    } else {
      qDebug() << QString("AdmVoiceMailWidget::sPlayMsgClicked: currentSource().type()=%1 state()=%2").arg(_mObj->currentSource().type()).arg(_mObj->state());
      if(_mObj->isSeekable())
      {
        if(_mObj->currentTime() >= _mObj->totalTime())
          _mObj->seek(0);
      }
      _mObj->play();
    }
  }
}
void AdmVoiceMailWidget::sMsgSeekBackClick()
{
  if(_mObj->isSeekable() && _mObj->state() == Phonon::PlayingState)
  {
    _mObj->seek( (_mObj->currentTime() - 1500) < 0
                  ? 0
                  : _mObj->currentTime() - 1500);
  }
}
void AdmVoiceMailWidget::sMsgSeekForwardClick()
{
  if(_mObj->isSeekable() && _mObj->state() == Phonon::PlayingState)
  {
    _mObj->seek( (_mObj->currentTime() + 1500) >= _mObj->totalTime()
                  ? _mObj->totalTime()
                  : _mObj->currentTime() + 1500);
  }
}

void AdmVoiceMailWidget::sSoundFileReady(const QByteArray &data)
{
  _mObj->clearQueue();
  _mObj->clear();

  //delete other wav files
  removeTmpMsgFiles();

  //QTemporaryFile *pf = _mFile;
  QTemporaryFile _mFile(QString("%1/qtadm_vm_msg.wav.XXXXXX").arg(QDir::tempPath()));
  _mFile.setAutoRemove(false);
  if(_mFile.open())
  {
    _mFile.write(data);
    _mFile.close();
    _mObj->enqueue(QUrl(QString("file://%1").arg(_mFile.fileName())));
    qDebug() << QString("AdmVoiceMailWidget::sSoundFileReady: currentSource().url()=%1").arg(_mObj->currentSource().url().toString());
    qDebug() << QString("AdmVoiceMailWidget::sSoundFileReady: before play state()=%1").arg(_mObj->state());
    _mObj->play();
    qDebug() << QString("AdmVoiceMailWidget::sSoundFileReady: after play state()=%1").arg(_mObj->state());
  }
  //if(NULL != pf)
  //{
    //delete pf;
    //pf = NULL;
  //}
}

void AdmVoiceMailWidget::sMediaObjectTick(qint64 time)
{
  qDebug() << QString("AdmVoiceMailWidget::sMediaObjectTick: time=%1").arg(time);
  QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
  ui->_lcdTime->display(displayTime.toString("mm:ss"));
}

void AdmVoiceMailWidget::sMediaObjectStateChanged(Phonon::State newState, Phonon::State oldState)
{
  qDebug() << QString("AdmVoiceMailWidget::sMediaObjectStateChanged: oldState=%1, newState=%2").arg(oldState).arg(newState);
  Q_UNUSED(oldState)
  switch (newState)
  {
    case Phonon::ErrorState:
    {
      if (_mObj->errorType() == Phonon::FatalError) {
          QMessageBox::warning(this, tr("Fatal Error"),
                              _mObj->errorString());
          //_mObj->setCurrentSource(_mSrc);
          _mObj->clearQueue();
          _mObj->clear();
      } else {

          QMessageBox::warning(this, tr("Error"),
          _mObj->errorString());
      }
      break;
    }
    case Phonon::PlayingState:
    {
      QIcon icon;
      icon.addFile(QString::fromUtf8(":/icons/media-playback-pause.png"), QSize(), QIcon::Normal, QIcon::Off);
      ui->_msgTogglePlay->setIcon(icon);
      break;
    }
    case Phonon::StoppedState:
    {
      QIcon icon;
      icon.addFile(QString::fromUtf8(":/icons/media-playback-start.png"), QSize(), QIcon::Normal, QIcon::Off);
      ui->_msgTogglePlay->setIcon(icon);
      ui->_lcdTime->display("00:00");
      break;
    }
    case Phonon::PausedState:
    {
      QIcon icon;
      icon.addFile(QString::fromUtf8(":/icons/media-playback-start.png"), QSize(), QIcon::Normal, QIcon::Off);
      ui->_msgTogglePlay->setIcon(icon);
      break;
    }
    default:
      break;
  }
}
void AdmVoiceMailWidget::sMediaObjectSourceChanged(Phonon::MediaSource src)
{
  qDebug() << QString("AdmVoiceMailWidget::sMediaObjectSourceChanged: type=%1, url=%2").arg(src.type()).arg(src.url().toString());

  ui->_lcdTime->display("00:00");
}
void AdmVoiceMailWidget::sMediaObjectFinished()
{
  qDebug() << QString("AdmVoiceMailWidget::sMediaObjectFinished");
  QIcon icon;
  icon.addFile(QString::fromUtf8(":/icons/media-playback-start.png"), QSize(), QIcon::Normal, QIcon::Off);
  ui->_msgTogglePlay->setIcon(icon);

  if(_mObj->isSeekable())
  {
    _mObj->pause();
    _mObj->seek(0);
  }
  else
  {
      _mObj->stop();
    //_mObj->setCurrentSource(_mSrc);
    //_mObj->clear();
  }
}

void AdmVoiceMailWidget::sPlayOnPhoneClicked()
{
  // TODO: Get the Playback path from the rest api using msg_id
  if(ui->_messages->selectedItems().count() > 0)
  {
    QVariant data = ui->_messages->item(ui->_messages->selectedItems().at(0)->row(),0)->data(Qt::UserRole);
    QVariantMap dataMap = data.toMap();
    emit sigPlayMsgOnPhone(this, dataMap);
  }
}

void AdmVoiceMailWidget::showNetworkErrorMsg(const QString& functionName, QNetworkReply::NetworkError err, const QString &errString)
{
  if(err != QNetworkReply::NoError)
  {
    qCritical() << QString("AdmVoiceMailWidget: QNetwork Error: [%1]: (%2) %3")
                   .arg(functionName)
                   .arg(QString::number(err))
                   .arg(errString);
    QMessageBox::critical(this,"Network Error",
                          QString("A network error occured: (%1) %2")
                          .arg(QString::number(err))
                          .arg(errString));
  }
}

void AdmVoiceMailWidget::sSendAmiVoicemailRefresh()
{
  QtAsteriskDesktopMain::getInstance()->sSendAmiVoicemailRefresh(_vmBox, _vmContext);
}

void AdmVoiceMailWidget::removeTmpMsgFiles()
{
  QDir dir(QDir::tempPath());
  dir.setNameFilters(QStringList() << "qtadm_vm_msg.wav.*");
  dir.setFilter(QDir::Files);
  foreach(QString dirFile, dir.entryList())
  {
    dir.remove(dirFile);
  }
}
