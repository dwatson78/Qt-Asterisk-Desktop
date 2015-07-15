#ifndef RESTAPIASTVM_H
#define RESTAPIASTVM_H

#include <QObject>
#include <QNetworkReply>

class RestApiAstVm : public QObject
{
  Q_OBJECT
public:
  static const int ROLE_FOLDER_NAME      = 1;
  static const int ROLE_FOLDER_MSG_COUNT = 2;

  explicit RestApiAstVm(QObject *parent = 0);
  ~RestApiAstVm();
  
  const QString& getActionId(){return _actionId;}
  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();


public slots:

protected:
  QString _actionId;
  bool _isDataStream;
  void _getRequest(const QString &api, const QVariantMap &headers);
  virtual void setRequest(const QNetworkRequest &req);
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setReady(const QNetworkRequest &req, const QByteArray &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

signals:
  void destroying(RestApiAstVm *obj);
  void sigReady(const QVariantMap &data);
  void sigReady(const QByteArray &data);
  void sigError(QNetworkReply::NetworkError err);

private:
  QNetworkAccessManager *_nam;
  QString _baseUrl;

private slots:
  void parseNetworkResponse(QNetworkReply *reply);

};

class RestApiAstVmMsgCounts : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmMsgCounts(QObject *parent = 0);
  ~RestApiAstVmMsgCounts();

  void set(const QVariantMap &values, bool *ok);
  void start();

protected:
  void setRequest(const QNetworkRequest &req);

  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

private:
  QString _vmBox;
  QNetworkRequest _req;

};

class RestApiAstVmMsgDetails : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmMsgDetails(QObject *parent = 0);
  ~RestApiAstVmMsgDetails();

  void set(const QVariantMap &values, bool *ok);
  void start();

protected:
  void setRequest(const QNetworkRequest &req);

  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

private:
  QString _vmBox;
  QString _vmFolder;
  QNetworkRequest _req;

};

class RestApiAstVmGetMsgSoundFile : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmGetMsgSoundFile(QObject *parent = 0);
  ~RestApiAstVmGetMsgSoundFile();

  void set(const QVariantMap &values, bool *ok);
  void start();

protected:
  void setRequest(const QNetworkRequest &req);

  virtual void setReady(const QNetworkRequest &req, const QByteArray &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

private:
  QString _vmBox;
  QString _vmFolder;
  QString _vmMsgId;
  QNetworkRequest _req;
};

class RestApiAstVmMoveMessage : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmMoveMessage(QObject *parent = 0);
  ~RestApiAstVmMoveMessage();

  void set(const QVariantMap &values, bool *ok);
  void start();

protected:
  void setRequest(const QNetworkRequest &req);

  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

private:
  QString _vmBox;
  QString _vmFolderSrc;
  QString _vmFolderDst;
  QString _vmMsgId;
  QNetworkRequest _req;
};

class RestApiAstVmForwardMessage : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmForwardMessage(QObject *parent = 0);
  ~RestApiAstVmForwardMessage();

  void set(const QVariantMap &values, bool *ok);
  void start();

protected:
  void setRequest(const QNetworkRequest &req);

  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

private:
  QString _vmBox;
  QString _vmFolder;
  QString _vmMsgId;
  QString _vmDestBox;
  QNetworkRequest _req;
};

class RestApiAstVmDeleteMessage : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmDeleteMessage(QObject *parent = 0);
  ~RestApiAstVmDeleteMessage();

  void set(const QVariantMap &values, bool *ok);
  void start();

protected:
  void setRequest(const QNetworkRequest &req);

  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err);

private:
  QString _vmBox;
  QString _vmFolder;
  QString _vmMsgId;
  QNetworkRequest _req;
};

#endif // RESTAPIASTVM_H
