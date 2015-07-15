#include "admvoicemailwidget.h"
#include "ui_admvoicemailwidget.h"
#include "restapiastvm.h"

#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QMessageBox>
#include <QMetaObject>
#include <QMetaEnum>
#include <QInputDialog>

AdmVoiceMailWidget::AdmVoiceMailWidget(QString vmBox, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AdmVoiceMailWidget)
{
  ui->setupUi(this);
  _vmBox = vmBox;
  _mObj = new Phonon::MediaObject();
  _mSrc = Phonon::MediaSource();
  _mOut = new Phonon::AudioOutput(Phonon::CommunicationCategory, this);
  Phonon::createPath(_mObj,_mOut);
  _mFile = NULL;
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
  delete ui;
  delete _mFile;
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

void AdmVoiceMailWidget::sVmCountError(QNetworkReply::NetworkError err)
{
  Q_UNUSED(err)
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
  params["vmBox"] = _vmBox;
  params["vmFolder"] = folder;
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
  _mObj->clear();
  if(NULL != _mFile)
  {
    //_mFile->remove();
    delete _mFile;
    _mFile = NULL;
  }
  ui->_frameSelectedMessage->setEnabled(NULL != current);
}

void AdmVoiceMailWidget::sMsgCustomContextMenuRequested(QPoint pos)
{
  if(ui->_messages->currentRow() < 0)
    return;
  QMenu *m = new QMenu();
  QMenu *m2 = new QMenu();
  m2 = m->addMenu("Move to");
  QString selectedFolder = ui->_folders->selectedItems().at(0)->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
  if(selectedFolder != "INBOX")
    m2->addAction(new QAction("New",this));
  if(selectedFolder != "Old")
    m2->addAction(new QAction("Old",this));
  if(selectedFolder != "Urgent")
    m2->addAction(new QAction("Urgent",this));
  if(selectedFolder != "Family")
    m2->addAction(new QAction("Family",this));
  if(selectedFolder != "Friends")
    m2->addAction(new QAction("Friends",this));
  if(selectedFolder != "Work")
    m2->addAction(new QAction("Work",this));
  m->addAction(new QAction("Forward to...", this));
  m->addSeparator();
  if(selectedFolder != "Deleted")
    m->addAction(new QAction("Archive",this));
  m->addAction(new QAction("Delete",this));

  connect(m,   SIGNAL(triggered(QAction*)),
          this, SLOT(sMsgCustomContextAction(QAction*)));
  connect(m2,   SIGNAL(triggered(QAction*)),
          this, SLOT(sMsgCustomContextMoveMsg(QAction*)));

  m->popup(ui->_messages->viewport()->mapToGlobal(pos));
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
      qDebug() << destBox;
      QVariantMap params;
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
        connect(fwd,  SIGNAL(sigError(QNetworkReply::NetworkError)),
                this, SLOT(sVmForwardMessageError(QNetworkReply::NetworkError))
        );
        fwd->start();
      }
    }
  } else if(action->text() == "Archive")
  {
    if(folder != "Deleted")
    {
      QAction *archive = new QAction("Deleted",this);
      sMsgCustomContextMoveMsg(archive);
    }
  } else if (action->text() == "Delete") {

    QVariantMap params;
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
      connect(r,    SIGNAL(sigError(QNetworkReply::NetworkError)),
              this, SLOT(sVmDeleteMessageError(QNetworkReply::NetworkError)));
      r->start();
    }
  }
}

void AdmVoiceMailWidget::sVmForwardMessageReady(const QVariantMap &data)
{
  Q_UNUSED(data);
}
void AdmVoiceMailWidget::sVmForwardMessageError(QNetworkReply::NetworkError err)
{
  Q_UNUSED(err);
  qDebug() << QString("AdmVoiceMailWidget::sVmForwardMessageError: %1").arg(err);
  
}

void AdmVoiceMailWidget::sVmDeleteMessageReady(const QVariantMap &data)
{
  Q_UNUSED(data);
  sRefreshView();
}
void AdmVoiceMailWidget::sVmDeleteMessageError(QNetworkReply::NetworkError err)
{
  Q_UNUSED(err);
  
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
    /*if(values.contains("vmBox")
     && values.contains("vmFolderSrc")
     && values.contains("vmFolderDst")
     && values.contains("vmFile"))*/
    params["vmBox"] = _vmBox;
    params["vmFolderSrc"] = ui->_folders->currentItem()->data(RestApiAstVm::ROLE_FOLDER_NAME).toString();
    params["vmFolderDst"] = action->text() == "New" ? "INBOX" : action->text();
    params["vmMsgId"] = dataMap.value("msg_id").toString();
    move->set(params,&ok);
    if(ok)
    {
      connect(move, SIGNAL(sigReady(QVariantMap)),
              this, SLOT(sVmMoveMessageReady(QVariantMap)));
      connect(move, SIGNAL(sigError(QNetworkReply::NetworkError)),
              this, SLOT(sVmMoveMessageError(QNetworkReply::NetworkError)));
      move->start();
    }
  }
}
void AdmVoiceMailWidget::sVmMoveMessageReady(const QVariantMap &data)
{
  Q_UNUSED(data)
  sRefreshView();
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
}

void AdmVoiceMailWidget::sVmMoveMessageError(QNetworkReply::NetworkError err)
{
  qDebug() << err;
}

void AdmVoiceMailWidget::sPlayMsgClicked()
{
  qDebug() << "AdmVoiceMailWidget::sPlayMsgClicked";
  if(_mObj->state() == Phonon::PlayingState)
  {
    qDebug() << "AdmVoiceMailWidget::sPlayMsgClicked: pause";
    _mObj->pause();
  } else {
    if(_mObj->currentSource().type() == Phonon::MediaSource::Empty
       || _mObj->currentSource().type() == 5)
    {
      qDebug() << QString("AdmVoiceMailWidget::sPlayMsgClicked: source type (%1)")
                  .arg(_mObj->currentSource().type())
                  ;
      qDebug() << "AdmVoiceMailWidget::sPlayMsgClicked: currentSource().type() == Phonon::MediaSource::Empty";
      if(ui->_messages->selectedItems().count() > 0)
      {
        qDebug() << QString("Selected row: %1").arg(ui->_messages->selectedItems().at(0)->row());
        QVariant data = ui->_messages->item(ui->_messages->selectedItems().at(0)->row(),0)->data(Qt::UserRole);
        QVariantMap dataMap = data.toMap();
        qDebug() << dataMap.value("sndfile");
        RestApiAstVmGetMsgSoundFile *r = new RestApiAstVmGetMsgSoundFile();
        bool ok = false;
        QVariantMap params;
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
          qDebug() << "AdmVoiceMailWidget::sPlayMsgClicked: r->set(...) NOT OK!!";
        }
      }
    } else {
      qDebug() << QString("AdmVoiceMailWidget::sPlayMsgClicked: source type (%1) was not (%2)")
                  .arg(_mObj->currentSource().type())
                  .arg(Phonon::MediaSource::Empty)
                  ;
      qDebug() << "AdmVoiceMailWidget::sPlayMsgClicked: play";
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
  qDebug() << _mObj->isSeekable();
  if(_mObj->isSeekable() && _mObj->state() == Phonon::PlayingState)
  {
    _mObj->seek( (_mObj->currentTime() - 1500) < 0
                  ? 0
                  : _mObj->currentTime() - 1500);
  }
}
void AdmVoiceMailWidget::sMsgSeekForwardClick()
{
  qDebug() << _mObj->isSeekable();
  if(_mObj->isSeekable() && _mObj->state() == Phonon::PlayingState)
  {
    _mObj->seek( (_mObj->currentTime() + 1500) >= _mObj->totalTime()
                  ? _mObj->totalTime()
                  : _mObj->currentTime() + 1500);
  }
}

void AdmVoiceMailWidget::sSoundFileReady(const QByteArray &data)
{
  qDebug() << data.length();
  _mObj->clear();
  if(NULL != _mFile)
  {
    delete _mFile;
    _mFile = NULL;
  }
  _mFile = new QTemporaryFile(QString("%1/msg.wav.XXXXXX").arg(QDir::tempPath()));
  if(_mFile->open())
  {
    qDebug() << QString("_mFile->fileName(): %1").arg(_mFile->fileName());
    _mFile->write(data);
    _mFile->close();
    _mObj->setCurrentSource(QUrl(QString("file://%1").arg(_mFile->fileName())));
    _mObj->play();
  }
}

void AdmVoiceMailWidget::sMediaObjectTick(qint64 time)
{
  QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
  ui->_lcdTime->display(displayTime.toString("mm:ss"));
}

void AdmVoiceMailWidget::sMediaObjectStateChanged(Phonon::State newState, Phonon::State oldState)
{
  Q_UNUSED(oldState)
  switch (newState)
  {
    case Phonon::ErrorState:
    {
      if (_mObj->errorType() == Phonon::FatalError) {
          QMessageBox::warning(this, tr("Fatal Error"),
          _mObj->errorString());
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
      //playAction->setEnabled(false);
      //pauseAction->setEnabled(true);
      //stopAction->setEnabled(true);
      break;
    }
    case Phonon::StoppedState:
    {
      //stopAction->setEnabled(false);
      //playAction->setEnabled(true);
      //pauseAction->setEnabled(false);
      //timeLcd->display("00:00");
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
void AdmVoiceMailWidget::sMediaObjectSourceChanged(Phonon::MediaSource)
{
  ui->_lcdTime->display("00:00");
}
void AdmVoiceMailWidget::sMediaObjectFinished()
{
  qDebug() << "AdmVoiceMailWidget::sMediaObjectFinished";
  QIcon icon;
  icon.addFile(QString::fromUtf8(":/icons/media-playback-start.png"), QSize(), QIcon::Normal, QIcon::Off);
  ui->_msgTogglePlay->setIcon(icon);
  _mObj->stop();
  if(_mObj->isSeekable())
    _mObj->seek(0);
  else
    _mObj->setCurrentSource(_mSrc);
}

void AdmVoiceMailWidget::sPlayOnPhoneClicked()
{
  // TODO: Get the Playback path from the rest api using msg_id
  if(ui->_messages->selectedItems().count() > 0)
  {
    qDebug() << QString("Selected row: %1").arg(ui->_messages->selectedItems().at(0)->row());
    QVariant data = ui->_messages->item(ui->_messages->selectedItems().at(0)->row(),0)->data(Qt::UserRole);
    QVariantMap dataMap = data.toMap();
    emit sigPlayMsgOnPhone(this, dataMap);
  }
}
