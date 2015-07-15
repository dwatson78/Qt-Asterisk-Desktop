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

  qDebug() << tr("host: '%1'").arg(AmiPref::getName(AmiPref::host));

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
}
