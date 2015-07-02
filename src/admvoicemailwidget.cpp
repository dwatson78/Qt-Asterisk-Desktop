#include "admvoicemailwidget.h"
#include "ui_admvoicemailwidget.h"
#include "restapiastvm.h"

#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QMessageBox>

AdmVoiceMailWidget::AdmVoiceMailWidget(QString vmBox, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AdmVoiceMailWidget)
{
  ui->setupUi(this);
  _vmBox = vmBox;
  _mObj = new Phonon::MediaObject();
  _mSrc = Phonon::MediaSource();
  _mOut = new Phonon::AudioOutput(Phonon::MusicCategory, this);
  Phonon::createPath(_mObj,_mOut);
  _mFile = NULL;

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
  //connect(_mObj,  SIGNAL(aboutToFinish()),
  //        this,   SLOT(sMediaObjectAboutToFinish()));
  connect(ui->_msgSeekBack, SIGNAL(clicked()),
          this,             SLOT(sMsgSeekBackClick()));
  connect(ui->_msgSeekForward,  SIGNAL(clicked()),
          this,                 SLOT(sMsgSeekForwardClick()));

  ui->_frameSelectedMessage->setEnabled(false);
  ui->_lcdTime->display("00:00");
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
  _mObj->clear();
  if(NULL != _mFile)
  {
    //_mFile->remove();
    delete _mFile;
    _mFile = NULL;
  }
  ui->_frameSelectedMessage->setEnabled(NULL != current);
}

void AdmVoiceMailWidget::sPlayMsgClicked()
{
  if(_mObj->state() == Phonon::PlayingState)
  {
    _mObj->pause();
  } else {
    if(_mObj->currentSource().type() == Phonon::MediaSource::Empty)
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
    } else {
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
  //This works
  /*QBuffer *buf = new QBuffer();
  buf->setData(data);
  buf->open(QIODevice::ReadOnly);
  _mSrc = Phonon::MediaSource(buf);
  _mObj->setCurrentSource(_mSrc);
  //_mObj->play();*/
  _mFile = new QTemporaryFile(QString("%1/msg.wav.XXXXXX").arg(QDir::tempPath()));
  if(_mFile->open())
  {
    qDebug() << QString("_mFile->fileName(): %1").arg(_mFile->fileName());
    _mFile->write(data);
    _mFile->close();
    _mObj->setCurrentSource(QUrl(QString("file://%1").arg(_mFile->fileName())));
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
void AdmVoiceMailWidget::sMediaObjectAboutToFinish()
{
  
}
