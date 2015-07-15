#include "qtasteriskdesktopmain.h"
#include "admstatic.h"
#include <QApplication>
#include <iostream>

void msgHandler(QtMsgType type, const char* msg)
{
  const QString symbols[] = {"DEBUG", "WARN", "ERROR", "FATAL"};
  QString ou = QString("%1 [%2]: %3")
      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"))
      .arg(symbols[type])
      .arg(msg);
  std::cerr << ou.toStdString() << std::endl;
  if(type == QtFatalMsg)
    abort();
}

int main(int argc, char *argv[])
{
  qInstallMsgHandler(msgHandler);
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("Watson Brothers");
  QCoreApplication::setApplicationName("QtAsteriskDesktop");
  QCoreApplication::setApplicationVersion("0.1.0");
  AdmStatic::getInstance();
  QtAsteriskDesktopMain w;
  w.show();
  
  return a.exec();
}

