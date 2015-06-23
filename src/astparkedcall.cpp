#include "astparkedcall.h"
#include "ui_astparkedcallwidget.h"
#include <QLabel>
#include <QIcon>
#include <QToolButton>
#include "admstatic.h"
#include "admicontext.h"

AstParkedCall::AstParkedCall(AstChannel *chanParked, AstChannel *chanFrom, QVariantMap event, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AstParkedCallWidget)
{
  ui->setupUi(this);

  // Start the clock
  _time = new QTime();
  _time->start();

  connect(AdmStatic::getInstance()
          ->getTimer(), SIGNAL(timeout()),
          this,         SLOT(sTickTock())
  );

  this->_chanParked = chanParked;
  connect(this->_chanParked, SIGNAL(destroying(AstChannel*)),
          this,SLOT(sDestroyChannel(AstChannel*))
  );

  this->_chanFrom   = chanFrom;
  connect(this->_chanFrom, SIGNAL(destroying(AstChannel*)),
          this,SLOT(sDestroyChannel(AstChannel*))
  );
  if(event.contains("Uniqueid"))
    this->_uuid = new QString(event.value("Uniqueid").toString());
  else
    this->_uuid = new QString();

  this->_isParked = true;
  if(event.contains("Exten"))
    _parkedExten = event.value("Exten").toUInt();
  if(event.contains("ConnectedLineName"))
    _parkedFromName = event.value("ConnectedLineName").toString();
  if(event.contains("ConnectedLineNum"))
    _parkedFromNum = event.value("ConnectedLineNum").toString();
  if(event.contains("From"))
    _parkedFromChannelName = event.value("From").toString();

  connect(ui->_tbAnswerParkedCall,  SIGNAL(clicked()),
          this,                     SLOT(sStartAnswerParkedCall())
  );

  ui->_lblExten->setText(_parkedExten.toString());
  ui->_parkInfo->setText(_chanParked->getCallIdStr());
  ui->_fromInfo->setText(getParkedFromStr());
}
AstParkedCall::~AstParkedCall()
{
  emit destroying(this);
}

void AstParkedCall::sParkedCallEvent(AsteriskManager::Event eventType, QVariantMap event)
{
  switch(eventType)
  {
    case AsteriskManager::ParkedCall:
      break;
    case AsteriskManager::UnParkedCall:
    {
      QString name;
      QVariant num(QVariant::UInt);
      if(event.contains("ConnectedLineName"))
        name = event.value("ConnectedLineName").toString();
      if(event.contains("ConnectedLineNum") && event.value("ConnectedLineNum").toString() != "")
        num = event.value("ConnectedLineNum").toUInt();
      if(!num.isNull())
      {
        this->ui->_statusInfo1->setText(QString("Answered: %1 <%2>").arg(name).arg(num.toString()));
      } else {
        this->ui->_statusInfo1->setText(QString("Answered: %1").arg(name));
      }
      disconnect(AdmStatic::getInstance()
                 ->getTimer(),  SIGNAL(timeout()),
              this,             SLOT(sTickTock())
      );
      this->setEnabled(false);
      break;
    }
    case AsteriskManager::ParkedCallGiveUp:
    {
      this->ui->_statusInfo1->setText("Parked caller hungup");
      disconnect(AdmStatic::getInstance()
                 ->getTimer(),  SIGNAL(timeout()),
              this,             SLOT(sTickTock())
      );
      this->setEnabled(false);
      break;
    }
    case AsteriskManager::ParkedCallTimeOut:
    {
      this->ui->_statusInfo1->setText("Parked call timeout");
      disconnect(AdmStatic::getInstance()
                 ->getTimer(),  SIGNAL(timeout()),
              this,             SLOT(sTickTock())
      );
      this->setEnabled(false);
      break;
    }
    default:
      break;
  }
}

void AstParkedCall::sParkTimeOut(QVariantMap event)
{
  emit parkTimeOut(this, event);
}

void AstParkedCall::sParkGiveUp(QVariantMap event)
{
  emit parkGiveUp(this, event);
}

void AstParkedCall::sDestroyChannel(AstChannel * channel)
{
  if(NULL != this->_chanFrom && channel == this->_chanFrom)
  {
    this->_chanFrom = NULL;
  }
  if(NULL != this->_chanParked && channel == this->_chanParked)
  {
    this->_chanParked = NULL;
  }
  if(NULL == this->_chanFrom && NULL == this->_chanParked)
  {
    this->deleteLater();
  }
}

uint AstParkedCall::getParkedExten(bool *valid)
{
  if(!this->_parkedExten.isNull())
  {
    *valid = true;
    return this->_parkedExten.toUInt();
  }
  *valid = false;
  return 0;
}

QString AstParkedCall::getParkedFromName()
{
  return this->_parkedFromName;
}

uint AstParkedCall::getParkedFromNum(bool *valid)
{
  if(!this->_parkedFromNum.isNull())
  {
    *valid = true;
    return this->_parkedFromNum.toUInt();
  }
  *valid = false;
  return 0;
}

QString AstParkedCall::getParkedFromChannel()
{
  return this->_parkedFromChannelName;

}

QString AstParkedCall::getParkedFromStr()
{
  if(_isParked)
  {
    if(!this->_parkedFromNum.isNull())
    {
      return QString("%1 <%2>").arg(this->_parkedFromName).arg(this->_parkedFromNum.toString());
    } else {
      return QString("%1").arg(this->_parkedFromName);
    }
  }
  return QString();
}

void AstParkedCall::sStartAnswerParkedCall()
{
  emit pickUpParkedCall(this);
}

void AstParkedCall::sTickTock()
{
  if(_time->elapsed() >= 33000)
  {
    QPixmap pix(":/icons/error.png");
    this->ui->_statusInfo3->setPixmap(pix);
    this->ui->_lblExten->setPixmap(pix);
  } else if(_time->elapsed() >= 20000) {
    QPixmap pix(":/icons/important.png");
    this->ui->_statusInfo3->setPixmap(pix);
    this->ui->_lblExten->setPixmap(pix);
  } else {
    //this->ui->_statusInfo3->setPixmap(QPixmap());
  }
  this->ui->_statusInfo3->setText(
        QString("Parked: %1")
        .arg(AdmStatic::elapsedTimeToString(_time))
  );
}
