#include "astparkedcall.h"
#include "ui_astparkedcallwidget.h"
#include <QLabel>
#include <QIcon>
#include <QToolButton>

AstParkedCall::AstParkedCall(AstChannel *chanParked, AstChannel *chanFrom, QVariantMap event, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AstParkedCallWidget)
{
  ui->setupUi(this);
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

  ui->_parkInfo->setText(QString("Slot: %1: %2")
                         .arg(_parkedExten.toString())
                         .arg(_chanParked->getCallIdStr())
  );

  ui->_fromInfo->setText(getParkedFromStr());

/*
  _lyt = new QGridLayout();

  //Add the data
  QLabel *_lblParkedLit = new QLabel("Parked: ");
  _lblParkedLit->setObjectName("_lblParkedLit");
  _lyt->addWidget(_lblParkedLit,0,0,1,1,(Qt::AlignLeading | Qt::AlignVCenter));

  QLabel *_lblFromLit = new QLabel("From: ");
  _lblFromLit->setObjectName("_lblFromLit");
  _lyt->addWidget(_lblFromLit,1,0,1,1,(Qt::AlignLeading | Qt::AlignVCenter));

  QLabel *_lblParked = new QLabel(QString("Slot: %1: %2")
                                  .arg(_parkedExten.toString())
                                  .arg(_chanParked->getCallIdStr())
  );
  _lblParked->setObjectName("_lblParked");
  _lyt->addWidget(_lblParked,0,1,1,1,(Qt::AlignLeading | Qt::AlignVCenter));

  QLabel *_lblFrom = new QLabel(QString("%1")
                                .arg(getParkedFromStr())
  );
  _lblFrom->setObjectName("_lblFrom");
  _lyt->addWidget(_lblFrom,1,1,1,1,(Qt::AlignLeading | Qt::AlignVCenter));

  //Set up the call buttons
  QHBoxLayout * _tbBox = new QHBoxLayout();
  _tbBox->setSpacing(2);

  // Set up Call Pickup
  QIcon iconCallPickup;
  iconCallPickup.addFile(QString::fromUtf8(":/icons/call-pickup.png"), QSize(), QIcon::Normal, QIcon::Off);
  QToolButton * _tbCallPickup = new QToolButton();
  _tbCallPickup->setObjectName(QString::fromUtf8("_tbCallPickup"));
  _tbCallPickup->setIcon(iconCallPickup);
  _tbCallPickup->setIconSize(QSize(24, 24));
  connect(_tbCallPickup,  SIGNAL(clicked()),
          this,           SLOT(sStartAnswerParkedCall())
  );
  _tbBox->addWidget(_tbCallPickup);

  // Finish setting up the call buttons
  _tbBox->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
  _lyt->addLayout(_tbBox,2,0,1,3,(Qt::AlignLeading | Qt::AlignVCenter));


  // Set up the Status Indicators
  QFrame *_frameStatus = new QFrame();
  _frameStatus->setObjectName("_frameStatus");
  _frameStatus->setFrameStyle(QFrame::StyledPanel);

  QGridLayout *_layoutStatus = new QGridLayout();
  _layoutStatus->setObjectName("_layoutStatus");
  _frameStatus->setLayout(_layoutStatus);

  QLabel * _lblStatus1 = new QLabel("Time on Park: ");
  _lblStatus1->setObjectName("_lblStatus1");
  _layoutStatus->addWidget(_lblStatus1,0,0,1,1,(Qt::AlignLeading | Qt::AlignVCenter));

  /* QSpacerItem *_hspacer1 = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum);
  _layoutStatus->addSpacerItem(_hspacer1);

  QLabel * _lblStatus2 = new QLabel("2");
  _lblStatus2->setObjectName("_lblStatus2");
  _layoutStatus->addWidget(_lblStatus2, 0, (Qt::AlignCenter | Qt::AlignVCenter));

  QSpacerItem *_hspacer2 = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum);
  _layoutStatus->addSpacerItem(_hspacer2);

  QLabel * _lblStatus3 = new QLabel("3");
  _lblStatus3->setObjectName("_lblStatus3");
  _layoutStatus->addWidget(_lblStatus3, 0, (Qt::AlignTrailing | Qt::AlignVCenter));
  * /

  _lyt->addWidget(_frameStatus,3,0,1,3,(Qt::AlignLeading | Qt::AlignVCenter));

  QSpacerItem *_hspacer3 = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum);
  _lyt->addItem(_hspacer3,0,2,2,1,(Qt::AlignLeading | Qt::AlignVCenter));

  this->setLayout(_lyt);
  */
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
      break;
    case AsteriskManager::ParkedCallGiveUp:
      break;
    case AsteriskManager::ParkedCallTimeOut:
      break;
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
