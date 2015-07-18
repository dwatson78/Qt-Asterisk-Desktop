#include "qtasteriskdesktopprefs.h"
#include "ui_qtasteriskdesktopprefs.h"

#include <QDebug>
#include <QMetaEnum>
#include <QSettings>

const QString & AmiHost()
{
	static const QString constant("AMI/host");
	return constant;
}

QtAsteriskDesktopPrefs::QtAsteriskDesktopPrefs(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::QtAsteriskDesktopPrefs)
{
  ui->setupUi(this);
  connect( ui->_buttonBox, SIGNAL(accepted()),
           this,           SLOT(sAccepted())
  );

  QSettings settings;
  settings.beginGroup("AMI");

  QString setting = AmiPref::getName(AmiPref::host);
  if(settings.contains(setting))
	ui->_amiHost->setText(settings.value(setting).toString());

  setting = AmiPref::getName(AmiPref::port);
  if(settings.contains(setting))
	ui->_amiPort->setText(settings.value(setting).toString());

  setting = AmiPref::getName(AmiPref::userName);
  if(settings.contains(setting))
	ui->_amiUserName->setText(settings.value(setting).toString());

  setting = AmiPref::getName(AmiPref::secPass);
  if(settings.contains(setting))
	ui->_amiPassword->setText(settings.value(setting).toString());

  setting = AmiPref::getName(AmiPref::connectInterval);
  if(settings.contains(setting))
	ui->_amiConnectInterval->setText(settings.value(setting).toString());
  
  settings.endGroup();
  
  settings.beginGroup("DEVICES");
  if(settings.contains("default"))
    ui->_device->setText(settings.value("default").toString());
  settings.endGroup();

  settings.beginGroup("RESTAPI");
  setting = RestApiPref::getName(RestApiPref::baseUrl);
  if(settings.contains(setting))
    ui->_restApiBaseUrl->setText(settings.value(setting).toString());
  settings.endGroup();
  
  settings.beginGroup("XMPP");

  setting = XmppPref::getName(XmppPref::host);
  if(settings.contains(setting))
    ui->_xmppHost->setText(settings.value(setting).toString());

  setting = XmppPref::getName(XmppPref::port);
  if(settings.contains(setting))
    ui->_xmppPort->setText(settings.value(setting).toString());

  setting = XmppPref::getName(XmppPref::allowAutoSubscribe);
  if(settings.contains(setting))
    ui->_xmppAllowAutoSubscribe->setChecked(settings.value(setting).toBool());
    
  setting = XmppPref::getName(XmppPref::userName);
  if(settings.contains(setting))
    ui->_xmppUserName->setText(settings.value(setting).toString());

  setting = XmppPref::getName(XmppPref::realm);
  if(settings.contains(setting))
    ui->_xmppRealm->setText(settings.value(setting).toString());

  setting = XmppPref::getName(XmppPref::secPass);
  if(settings.contains(setting))
    ui->_xmppPassword->setText(settings.value(setting).toString());

  settings.endGroup();
}

QtAsteriskDesktopPrefs::~QtAsteriskDesktopPrefs()
{
  delete ui;
}

void QtAsteriskDesktopPrefs::sAccepted()
{
	  QSettings settings;

	  settings.beginGroup("AMI");
	  settings.setValue(AmiPref::getName(AmiPref::host),ui->_amiHost->text());
	  settings.setValue(AmiPref::getName(AmiPref::port),ui->_amiPort->text());
	  settings.setValue(AmiPref::getName(AmiPref::userName),ui->_amiUserName->text());
	  settings.setValue(AmiPref::getName(AmiPref::secPass),ui->_amiPassword->text());
	  settings.setValue(AmiPref::getName(AmiPref::connectInterval),ui->_amiConnectInterval->text());
	  settings.endGroup();

	  settings.beginGroup("DEVICES");
	  settings.setValue("default",ui->_device->text());
	  settings.endGroup();

    settings.beginGroup("RESTAPI");
    settings.setValue(RestApiPref::getName(RestApiPref::baseUrl),ui->_restApiBaseUrl->text());
    settings.endGroup();
    
    settings.beginGroup("XMPP");
    settings.setValue(XmppPref::getName(XmppPref::host),ui->_xmppHost->text());
    settings.setValue(XmppPref::getName(XmppPref::port),ui->_xmppPort->text());
    settings.setValue(XmppPref::getName(XmppPref::allowAutoSubscribe),ui->_xmppAllowAutoSubscribe->isChecked());
    settings.setValue(XmppPref::getName(XmppPref::userName),ui->_xmppUserName->text());
    settings.setValue(XmppPref::getName(XmppPref::realm),ui->_xmppRealm->text());
    settings.setValue(XmppPref::getName(XmppPref::secPass),ui->_xmppPassword->text());
    settings.endGroup();
}
