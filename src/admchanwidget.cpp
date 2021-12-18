#include "admchanwidget.h"
#include "ui_admchanwidget.h"

#include "admicontext.h"
#include "admstatic.h"
#include "astdialogshowevents.h"

#include <QMenu>
#include <QAction>
#include <QPlainTextEdit>
#include <QStackedWidget>
#include <QListWidget>
#include <QWaitCondition>

AdmChanWidget::AdmChanWidget(AstChannel *chan, QFrame *parent) :
  QFrame(parent),
  ui(new Ui::AdmChanWidget)
{
  ui->setupUi(this);
  ui->_frameMore->setVisible(false);

  QFont moreTextFont = ui->_lblMore->getTextLabel()->font();
  moreTextFont.setPointSize(8);
  moreTextFont.setItalic(true);
  ui->_lblMore->getTextLabel()->setFont(moreTextFont);

  _chan = chan;
  _isMusicShowing = false;
  if(NULL != _chan)
  {
    setText(_chan->getCallIdStr());
    setMoreText(_chan->getChannel());
    sShowHideMusic();
  }
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(sCustomContextMenu(const QPoint &)));
  //QMutex _mutex(QMutex::Recursive);
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

void AdmChanWidget::sCustomContextMenu(const QPoint &point)
{
    QMenu contextMenu("Context menu", this);
    QAction actionShowEvents("Show Events", this);
    connect(&actionShowEvents,  SIGNAL(triggered(bool)),
            this,               SLOT(sShowEvents(bool)));
    contextMenu.addAction(&actionShowEvents);
    contextMenu.exec(mapToGlobal(point));
}

void AdmChanWidget::sShowEvents(bool triggered)
{
  Q_UNUSED(triggered);

  AstDialogShowEvents *dlg = new AstDialogShowEvents(static_cast<QWidget*>(this->parent()));
  QList<QVariantMap> events = _chan->getEvents();
  /*QStackedWidget *_events = dlg->findChild<QStackedWidget *>("_events");
  QListWidget *_eventList = dlg->findChild<QListWidget *>("_eventList");

  QListIterator<QVariantMap> i(events);
  int row = -1;
  while(i.hasNext())
  {
      QVariantMap event = i.next();
      if(_events != NULL)
      {
          QWidget *page = new QWidget;
          QPlainTextEdit *textEdit = new QPlainTextEdit;
          textEdit->setPlainText(AdmStatic::eventToString(event));
          textEdit->setReadOnly(true);

          QVBoxLayout *layout = new QVBoxLayout;
          layout->addWidget(textEdit);
          page->setLayout(layout);
          row++;
          _events->insertWidget(row, page);
          _eventList->insertItem(row, event.value("Event").toString());
      }
  }*/
  dlg->loadEvents(events);
  dlg->setModal(true);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->exec();
}
