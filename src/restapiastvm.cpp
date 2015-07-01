#include "restapiastvm.h"
#include "admstatic.h"
#include "qtasteriskdesktopprefs.h"

#include <qjson/parser.h>

#include <QDebug>
#include <QVariantMap>
#include <QUrl>
#include <QUuid>
#include <QSettings>

RestApiAstVm::RestApiAstVm(QObject *parent) :
  QObject(parent)
{
  _nam = AdmStatic::getInstance()->getNetAccessMgr();

  connect(_nam, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(parseNetworkResponse(QNetworkReply*)));

  QSettings set;
  set.beginGroup("RESTAPI");
  if(set.contains(RestApiPref::getName(RestApiPref::baseUrl)))
  {
    _baseUrl = set.value(RestApiPref::getName(RestApiPref::baseUrl)).toString();
    if(!_baseUrl.endsWith("/"))
      _baseUrl.append("/");
  } else {
    _baseUrl = QString();
  }
  set.endGroup();

  _actionId = QUuid::createUuid().toString();
}
RestApiAstVm::~RestApiAstVm()
{
  emit destroying(this);
}

void RestApiAstVm::_getRequest(const QString &api, const QVariantMap &headers)
{
  QUrl url(_baseUrl.append(api));
  for(QVariantMap::const_iterator i = headers.begin(); i != headers.end(); ++i)
    url.addQueryItem(i.key(),i.value().toString());
  qDebug() << url.toEncoded();
  QNetworkRequest req(url);
  setRequest(req);
  _nam->get(req);
}

void RestApiAstVm::parseNetworkResponse(QNetworkReply *reply)
{
  reply->request();
  if(reply->error() != QNetworkReply::NoError)
  {
    setError(reply->request(),reply->error());
    return;
  }

  QByteArray data = reply->readAll();
  QJson::Parser prsr;
  bool ok;
  QVariantMap dataMap = prsr.parse(data, &ok).toMap();
  if(ok)
  {
    setReady(reply->request(), dataMap);
  } else {
    setError(reply->request(), QNetworkReply::UnknownContentError);
  }
}

void RestApiAstVm::set(const QVariantMap &values, bool *ok)
{
  Q_UNUSED(values)
  Q_UNUSED(ok)
  // Implemented by derived class...
}

void RestApiAstVm::start()
{
  // Implemented by derived class...
}

void RestApiAstVm::setRequest(const QNetworkRequest &req)
{
  Q_UNUSED(req)
  // Implemented by derived class...

}
void RestApiAstVm::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  Q_UNUSED(req)
  Q_UNUSED(data)
  // Implemented by derived class...
}

void RestApiAstVm::setError(const QNetworkRequest &req, QNetworkReply::NetworkError err)
{
  Q_UNUSED(req)
  Q_UNUSED(err)
  // Implemented by derived class...
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmMsgCounts::RestApiAstVmMsgCounts(QObject *parent) :
  RestApiAstVm(parent)
{
  _vmBox = QString();
}
RestApiAstVmMsgCounts::~RestApiAstVmMsgCounts()
{

}

void RestApiAstVmMsgCounts::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(values.contains("vmBox"))
  {
    _vmBox = values.value("vmBox").toString();
    if(!_vmBox.isNull())
      *ok = true;
  }
}

void RestApiAstVmMsgCounts::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"] = "getVmCount";
  headers["actionid"] = _actionId;
  headers["vmbox"] = _vmBox;
  _getRequest(api, headers);
}
void RestApiAstVmMsgCounts::setRequest(const QNetworkRequest &req)
{
  _req = req;
}

void RestApiAstVmMsgCounts::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}

void RestApiAstVmMsgCounts::setError(const QNetworkRequest &req, QNetworkReply::NetworkError err)
{
  if(req == _req)
  {
    emit sigError(err);
    deleteLater();
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmMsgDetails::RestApiAstVmMsgDetails(QObject *parent) :
  RestApiAstVm(parent)
{
  _vmBox = QString();
  _vmFolder = QString();
}
RestApiAstVmMsgDetails::~RestApiAstVmMsgDetails()
{

}

void RestApiAstVmMsgDetails::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(values.contains("vmBox") && values.contains("vmFolder"))
  {
    _vmBox = values.value("vmBox").toString();
    _vmFolder = values.value("vmFolder").toString();
    if(!_vmBox.isNull() && !_vmFolder.isNull())
      *ok = true;
  }
}

void RestApiAstVmMsgDetails::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]   = "getVmMsgDetails";
  headers["actionid"] = _actionId;
  headers["vmbox"]    = _vmBox;
  headers["vmfolder"] = _vmFolder;
  _getRequest(api, headers);
}
void RestApiAstVmMsgDetails::setRequest(const QNetworkRequest &req)
{
  _req = req;
}

void RestApiAstVmMsgDetails::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}

void RestApiAstVmMsgDetails::setError(const QNetworkRequest &req, QNetworkReply::NetworkError err)
{
  if(req == _req)
  {
    emit sigError(err);
    deleteLater();
  }
}
