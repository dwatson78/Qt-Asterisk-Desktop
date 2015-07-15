#ifndef ASTCHANPARTS_H
#define ASTCHANPARTS_H

#include <QObject>
#include <QRegExp>

class AstChanParts : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString  Prefix  READ getPrefix   WRITE setPrefix   NOTIFY PrefixChanged)
  Q_PROPERTY(QString  Type  READ getType   WRITE setType   NOTIFY TypeChanged)
  Q_PROPERTY(QString  Exten READ getExten  WRITE setExten  NOTIFY ExtenChanged)
  Q_PROPERTY(QString  Extra READ getExtra  WRITE setExtra  NOTIFY ExtraChanged)
  Q_PROPERTY(QString  Special READ getSpecial  WRITE setSpecial  NOTIFY SpecialChanged)
  Q_PROPERTY(QBool    IsValid READ isValid)
  Q_PROPERTY(QBool    IsMyDevice READ isMyDevice)

public:
  explicit AstChanParts(QString channel, QObject *parent = 0);

  QString getPrefix();
  void setPrefix(QString prefix);

  QString getType();
  void setType(QString type);

  QString getExten();
  void setExten(QString exten);

  QString getExtra();
  void setExtra(QString extra);

  QString getSpecial();
  void setSpecial(QString special);

  QBool isValid();

  QBool isMyDevice();

protected:

signals:
  void PrefixChanged(const QString &newText);
  void TypeChanged(const QString &newText);
  void ExtenChanged(const QString &newText);
  void ExtraChanged(const QString &newText);
  void SpecialChanged(const QString &newText);

private:
  QString _prefix;
  QString _type;
  QString _exten;
  QString _extra;
  QString _special;
  bool _isValid;
};

#endif // ASTCHANPARTS_H
