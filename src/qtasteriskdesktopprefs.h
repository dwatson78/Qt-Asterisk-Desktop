#ifndef QTASTERISKDESKTOPPREFS_H
#define QTASTERISKDESKTOPPREFS_H

#include <QDialog>
#include <QMetaObject>
#include <QMetaEnum>

namespace Ui {
class QtAsteriskDesktopPrefs;
}

class AmiPref : public QObject
{
  Q_OBJECT
  Q_ENUMS(prefs)
public:
  enum prefs {host,port,userName,secPass,connectInterval};
  static QString getName(prefs pref){
    const QMetaObject &mo = AmiPref::staticMetaObject;
    int index = mo.indexOfEnumerator("prefs");
    QMetaEnum me = mo.enumerator(index);
    return QString(me.valueToKey(pref));
  }
};

class RestApiPref : public QObject
{
  Q_OBJECT
  Q_ENUMS(prefs)
public:
  enum prefs {baseUrl};
  static QString getName(prefs pref){
    const QMetaObject &mo = RestApiPref::staticMetaObject;
    int index = mo.indexOfEnumerator("prefs");
    QMetaEnum me = mo.enumerator(index);
    return QString(me.valueToKey(pref));
  }
};

class QtAsteriskDesktopPrefs : public QDialog
{
  Q_OBJECT
public:
  explicit QtAsteriskDesktopPrefs(QWidget *parent = 0);
  ~QtAsteriskDesktopPrefs();

public slots:
  void sAccepted();
 
private:
  Ui::QtAsteriskDesktopPrefs *ui;
  static const QString &_AmiHost;
  
};

#endif // QTASTERISKDESKTOPPREFS_H
