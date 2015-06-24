#include "admchanwidget.h"
#include "ui_admchanwidget.h"

#include "admicontext.h"

AdmChanWidget::AdmChanWidget(AstChannel *chan, QFrame *parent) :
  QFrame(parent),
  ui(new Ui::AdmChanWidget)
{
  ui->setupUi(this);
  ui->_frameMore->setVisible(false);
  _chan = chan;
  _isMusicShowing = false;
  if(NULL != _chan)
  {
    setText(_chan->getCallIdStr());
    sShowHideMusic();
  }
}

AdmChanWidget::~AdmChanWidget()
{
  emit destroying(this);
  delete ui;
}

const QString AdmChanWidget::getText()
{
  return ui->_lblTop->getText();
}
void AdmChanWidget::setText(const QString &text)
{
  ui->_lblTop->setText(text);
}

const QPixmap AdmChanWidget::getPixmap()
{
  return ui->_lblTop->getPixmap();
}
void AdmChanWidget::setPixmap(const QPixmap &pixmap)
{
  ui->_lblTop->setPixmap(pixmap);
}

const QString AdmChanWidget::getMoreText()
{
  return ui->_lblMore->getText();
}
void AdmChanWidget::setMoreText(const QString &text)
{
  ui->_lblMore->setText(text);
  ui->_frameMore->setVisible(ui->_lblMore->getText().trimmed().isEmpty() == false || ui->_lblMore->getPixmap().isNull() == false);
}

const QPixmap AdmChanWidget::getMorePixmap()
{
  return ui->_lblMore->getPixmap();
}
void AdmChanWidget::setMorePixmap(const QPixmap &pixmap)
{
  ui->_lblMore->setPixmap(pixmap);
  ui->_frameMore->setVisible(ui->_lblMore->getText().trimmed().isEmpty() == false || ui->_lblMore->getPixmap().isNull() == false);
}

void AdmChanWidget::sUpdateChannel()
{
  sShowHideMusic();
}

void AdmChanWidget::sShowHideMusic()
{
  if(_isMusicShowing == false && _chan->getIsMusicOn() == true)
  {
    QPixmap pm(":/icons/music-indicator.png");
    setPixmap(pm);
    _isMusicShowing = true;
  } else if (_isMusicShowing == true && _chan->getIsMusicOn() == false) {
    setPixmap(QPixmap());
    _isMusicShowing = false;
  }
}
