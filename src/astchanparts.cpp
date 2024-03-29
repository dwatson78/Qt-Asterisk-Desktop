#include "astchanparts.h"

#include <QSettings>

AstChanParts::AstChanParts(QString channel, QObject *parent)
  :QObject(parent)
{
  _isValid = false;
  _prefix=QString();
  _type=QString();
  _exten=QString();
  _extra=QString();
  _special=QString();
  QRegExp re ("^([^/]+/)?([^/]+)/([^-]+)([-])?([^<]+)?<?([^>]+)?>?$");
  if(re.exactMatch(channel) && re.captureCount() == 6)
  {
    this->setPrefix(re.cap(1));
    this->setType(re.cap(2));
    this->setExten(re.cap(3));
    this->setExtra(re.cap(5));
    this->setSpecial(re.cap(6));
    if(!_type.isNull() && !_type.isEmpty() && !_exten.isNull() && !_exten.isEmpty())
      _isValid = true;
  }
}

QString AstChanParts::getPrefix(){return _prefix;}
void AstChanParts::setPrefix(QString prefix){_prefix = prefix.replace("/","");}

QString AstChanParts::getType(){return _type;}
void AstChanParts::setType(QString type){_type = type;}

QString AstChanParts::getExten(){return _exten;}
void AstChanParts::setExten(QString exten){_exten = exten;}

QString AstChanParts::getExtra(){return _extra;}
void AstChanParts::setExtra(QString extra){_extra = extra;}

QString AstChanParts::getSpecial(){return _special;}
void AstChanParts::setSpecial(QString special){_special = special.replace("<","");}

bool AstChanParts::isValid(){return bool(_isValid);}

bool AstChanParts::isMyDevice(){
  if(_isValid)
  {
    QSettings set;
    if(set.contains("DEVICES/default"))
    {
      return bool(set.value("DEVICES/default").toString() == QString("%1/%2").arg(getType()).arg(getExten()));
    }
  }
  return false;
}
