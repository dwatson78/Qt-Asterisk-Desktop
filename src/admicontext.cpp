#include "admicontext.h"
#include "ui_admicontext.h"

AdmIconText::AdmIconText(const QPixmap &pixmap, const QString &text, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AdmIconText)
{
  ui->setupUi(this);
  ui->_icon->setVisible(false);

  if(!pixmap.isNull())
    setPixmap(pixmap);

  if(!text.isNull())
    setText(text);
}
AdmIconText::AdmIconText(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AdmIconText)
{
  ui->setupUi(this);
  ui->_icon->setVisible(false);
}

AdmIconText::~AdmIconText()
{
  delete ui;
}

QLabel *AdmIconText::getIconLabel()
{
  return this->ui->_icon;
}

QLabel *AdmIconText::getTextLabel()
{
  return this->ui->_text;
}

const QPixmap AdmIconText::getPixmap()
{
  return QPixmap(this->ui->_icon->pixmap()->copy(this->ui->_icon->pixmap()->rect()));
}
void AdmIconText::setPixmap(const QPixmap &pixmap)
{
  this->ui->_icon->setPixmap(pixmap);
  this->ui->_icon->setVisible(pixmap.isNull() == false);
}

const QString AdmIconText::getText()
{
  return this->ui->_text->text();
}
void AdmIconText::setText(const QString &text)
{
  this->ui->_text->setText(text);
}
