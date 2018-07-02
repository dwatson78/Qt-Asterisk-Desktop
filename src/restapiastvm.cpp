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
  _isDataStream = false;
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
  QNetworkRequest req(url);
  _req = req;
  _nam->get(_req);
}

void RestApiAstVm::parseNetworkResponse(QNetworkReply *reply)
{
  reply->request();
  if(reply->error() != QNetworkReply::NoError)
  {
    setError(reply->request(),reply->error(), reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();

  if(!_isDataStream)
  {
    QJson::Parser prsr;
    bool ok;
    QVariantMap dataMap = prsr.parse(data, &ok).toMap();
    if(ok)
    {
      setReady(reply->request(), dataMap);
    } else {
      setError(reply->request(), QNetworkReply::UnknownContentError, "Streamed data is unrecognized");
    }
  } else {
    setReady(reply->request(), data);
  }

}

void RestApiAstVm::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  Q_UNUSED(req)
  Q_UNUSED(data)
  // Implemented by derived class...
}
void RestApiAstVm::setReady(const QNetworkRequest &req, const QByteArray &data)
{
  Q_UNUSED(req)
  Q_UNUSED(data)
  // Implemented by derived class...
}

void RestApiAstVm::setError(const QNetworkRequest &req, QNetworkReply::NetworkError err, const QString &errString)
{
  if(req == _req && err != QNetworkReply::NoError)
  {
    emit sigError(err, errString);
    deleteLater();
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmMsgCounts::RestApiAstVmMsgCounts(QObject *parent) :
  RestApiAstVm(parent)
{
  _vmContext = QString();
  _vmBox = QString();
}
RestApiAstVmMsgCounts::~RestApiAstVmMsgCounts()
{

}

void RestApiAstVmMsgCounts::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(   values.contains("vmContext")
     && values.contains("vmBox"))
  {
    _vmContext = values.value("vmContext").toString();
    _vmBox = values.value("vmBox").toString();
    if(!_vmContext.isNull() && !_vmBox.isNull())
      *ok = true;
  }
}

void RestApiAstVmMsgCounts::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]     = "getVmCount";
  headers["actionid"]   = _actionId;
  headers["vmcontext"]  = _vmContext;
  headers["vmbox"]      = _vmBox;
  _getRequest(api, headers);
}

void RestApiAstVmMsgCounts::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
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
  _vmContext = QString();
  _vmBox = QString();
  _vmFolder = QString();
}
RestApiAstVmMsgDetails::~RestApiAstVmMsgDetails()
{

}

void RestApiAstVmMsgDetails::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(   values.contains("vmContext")
     && values.contains("vmBox")
     && values.contains("vmFolder"))
  {
    _vmContext = values.value("vmContext").toString();
    _vmBox = values.value("vmBox").toString();
    _vmFolder = values.value("vmFolder").toString();
    if(!_vmContext.isNull() && !_vmBox.isNull() && !_vmFolder.isNull())
      *ok = true;
  }
}

void RestApiAstVmMsgDetails::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]     = "getVmMsgDetails";
  headers["actionid"]   = _actionId;
  headers["vmcontext"]  = _vmContext;
  headers["vmbox"]      = _vmBox;
  headers["vmfolder"]   = _vmFolder;
  _getRequest(api, headers);
}

void RestApiAstVmMsgDetails::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmGetMsgSoundFile::RestApiAstVmGetMsgSoundFile(QObject *parent) :
  RestApiAstVm(parent)
{
  _isDataStream = true;
  _vmContext = QString();
  _vmBox = QString();
  _vmFolder = QString();
  _vmMsgId = QString();
}
RestApiAstVmGetMsgSoundFile::~RestApiAstVmGetMsgSoundFile()
{

}

void RestApiAstVmGetMsgSoundFile::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(   values.contains("vmContext") 
     && values.contains("vmBox")
     && values.contains("vmFolder")
     && values.contains("vmMsgId"))
  {
    _vmContext = values.value("vmContext").toString();
    _vmBox = values.value("vmBox").toString();
    _vmFolder = values.value("vmFolder").toString();
    _vmMsgId = values.value("vmMsgId").toString();
    if(!_vmContext.isNull() && !_vmBox.isNull() && !_vmFolder.isNull() && !_vmMsgId.isNull())
      *ok = true;
  }
}

void RestApiAstVmGetMsgSoundFile::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]     = "getVmMsgSoundFile";
  headers["actionid"]   = _actionId;
  headers["vmcontext"]  = _vmContext;
  headers["vmbox"]      = _vmBox;
  headers["vmfolder"]   = _vmFolder;
  headers["vmmsgid"]    = _vmMsgId;
  _getRequest(api, headers);
}

void RestApiAstVmGetMsgSoundFile::setReady(const QNetworkRequest &req, const QByteArray &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmMoveMessage::RestApiAstVmMoveMessage(QObject *parent) :
  RestApiAstVm(parent)
{
  _vmContext = QString();
  _vmBox = QString();
  _vmFolderSrc = QString();
  _vmFolderDst = QString();
  _vmMsgId = QString();
}
RestApiAstVmMoveMessage::~RestApiAstVmMoveMessage()
{

}

void RestApiAstVmMoveMessage::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(   values.contains("vmContext")
     && values.contains("vmBox")
     && values.contains("vmFolderSrc")
     && values.contains("vmFolderDst")
     && values.contains("vmMsgId"))
  {
    _vmContext = values.value("vmContext").toString();
    _vmBox = values.value("vmBox").toString();
    _vmFolderSrc = values.value("vmFolderSrc").toString();
    _vmFolderDst = values.value("vmFolderDst").toString();
    _vmMsgId = values.value("vmMsgId").toString();
    if(!_vmContext.isNull() && !_vmBox.isNull() && !_vmFolderSrc.isNull() && !_vmFolderDst.isNull() && !_vmMsgId.isNull())
      *ok = true;
  }
}

void RestApiAstVmMoveMessage::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]       = "moveVmMsg";
  headers["actionid"]     = _actionId;
  headers["vmcontext"]    = _vmContext;
  headers["vmbox"]        = _vmBox;
  headers["vmfoldersrc"]  = _vmFolderSrc;
  headers["vmfolderdst"]  = _vmFolderDst;
  headers["vmmsgid"]      = _vmMsgId;
  _getRequest(api, headers);
}

void RestApiAstVmMoveMessage::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmDeleteMessage::RestApiAstVmDeleteMessage(QObject *parent) :
  RestApiAstVm(parent)
{
  _vmContext = QString();
  _vmBox = QString();
  _vmFolder = QString();
  _vmMsgId = QString();
}
RestApiAstVmDeleteMessage::~RestApiAstVmDeleteMessage()
{

}

void RestApiAstVmDeleteMessage::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(   values.contains("vmContext")
     && values.contains("vmBox")
     && values.contains("vmFolder")
     && values.contains("vmMsgId"))
  {
    _vmContext = values.value("vmContext").toString();
    _vmBox = values.value("vmBox").toString();
    _vmFolder = values.value("vmFolder").toString();
    _vmMsgId = values.value("vmMsgId").toString();
    if(!_vmContext.isNull() && !_vmBox.isNull() && !_vmFolder.isNull() && !_vmMsgId.isNull())
      *ok = true;
  }
}

void RestApiAstVmDeleteMessage::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]     = "rmVmMsg";
  headers["actionid"]   = _actionId;
  headers["vmcontext"]  = _vmContext;
  headers["vmbox"]      = _vmBox;
  headers["vmfolder"]   = _vmFolder;
  headers["vmmsgid"]    = _vmMsgId;
  _getRequest(api, headers);
}

void RestApiAstVmDeleteMessage::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RestApiAstVmForwardMessage::RestApiAstVmForwardMessage(QObject *parent) :
  RestApiAstVm(parent)
{
  _vmContext = QString();
  _vmBox = QString();
  _vmFolder = QString();
  _vmMsgId = QString();
  _vmDestBox = QString();
}
RestApiAstVmForwardMessage::~RestApiAstVmForwardMessage()
{

}

void RestApiAstVmForwardMessage::set(const QVariantMap &values, bool *ok)
{
  *ok = false;
  if(   values.contains("vmContext")
     && values.contains("vmBox")
     && values.contains("vmFolder")
     && values.contains("vmMsgId")
     && values.contains("vmDestBox"))
  {
    _vmContext = values.value("vmContext").toString();
    _vmBox = values.value("vmBox").toString();
    _vmFolder = values.value("vmFolder").toString();
    _vmMsgId = values.value("vmMsgId").toString();
    _vmDestBox = values.value("vmDestBox").toString();
    if(!_vmContext.isNull() && !_vmBox.isNull() && !_vmFolder.isNull() && !_vmMsgId.isNull()  && !_vmDestBox.isNull())
      *ok = true;
  }
}

void RestApiAstVmForwardMessage::start()
{
  QString api = "vm.php";
  QVariantMap headers;
  headers["action"]     = "fwdVmMsg";
  headers["actionid"]   = _actionId;
  headers["vmcontext"]  = _vmContext;
  headers["vmbox"]      = _vmBox;
  headers["vmfolder"]   = _vmFolder;
  headers["vmmsgid"]    = _vmMsgId;
  headers["vmdestbox"]  = _vmDestBox;
  _getRequest(api, headers);
}

void RestApiAstVmForwardMessage::setReady(const QNetworkRequest &req, const QVariantMap &data)
{
  if(req == _req)
  {
    emit sigReady(data);
    deleteLater();
  }
}
