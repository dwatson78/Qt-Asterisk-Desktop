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
  virtual void set(const QVariantMap &values, bool *ok)=0;
  virtual void start()=0;


public slots:

protected:
  QString _actionId;
  bool _isDataStream;
  QNetworkRequest _req;

  void _getRequest(const QString &api, const QVariantMap &headers);
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);
  virtual void setReady(const QNetworkRequest &req, const QByteArray &data);
  virtual void setError(const QNetworkRequest &req, QNetworkReply::NetworkError err, const QString &errString);

signals:
  void destroying(RestApiAstVm *obj);
  void sigReady(const QVariantMap &data);
  void sigReady(const QByteArray &data);
  void sigError(QNetworkReply::NetworkError err, const QString &errString);

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

  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();

protected:
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);

private:
  QString _vmBox;


};

class RestApiAstVmMsgDetails : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmMsgDetails(QObject *parent = 0);
  ~RestApiAstVmMsgDetails();

  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();

protected:
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);

private:
  QString _vmBox;
  QString _vmFolder;
};

class RestApiAstVmGetMsgSoundFile : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmGetMsgSoundFile(QObject *parent = 0);
  ~RestApiAstVmGetMsgSoundFile();

  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();

protected:
  virtual void setReady(const QNetworkRequest &req, const QByteArray &data);

private:
  QString _vmBox;
  QString _vmFolder;
  QString _vmMsgId;
};

class RestApiAstVmMoveMessage : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmMoveMessage(QObject *parent = 0);
  ~RestApiAstVmMoveMessage();

  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();

protected:
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);

private:
  QString _vmBox;
  QString _vmFolderSrc;
  QString _vmFolderDst;
  QString _vmMsgId;
};

class RestApiAstVmForwardMessage : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmForwardMessage(QObject *parent = 0);
  ~RestApiAstVmForwardMessage();

  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();

protected:
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);

private:
  QString _vmBox;
  QString _vmFolder;
  QString _vmMsgId;
  QString _vmDestBox;
};

class RestApiAstVmDeleteMessage : public RestApiAstVm
{
  Q_OBJECT
public:
  explicit RestApiAstVmDeleteMessage(QObject *parent = 0);
  ~RestApiAstVmDeleteMessage();

  virtual void set(const QVariantMap &values, bool *ok);
  virtual void start();

protected:
  virtual void setReady(const QNetworkRequest &req, const QVariantMap &data);

private:
  QString _vmBox;
  QString _vmFolder;
  QString _vmMsgId;
};

#endif // RESTAPIASTVM_H
