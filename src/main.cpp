#include "qtasteriskdesktopmain.h"
#include "admstatic.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("Watson Brothers");
  QCoreApplication::setApplicationName("QtAsteriskDesktop");
  QCoreApplication::setApplicationVersion("0.0.1");
  AdmStatic::getInstance();
  QtAsteriskDesktopMain w;
  w.show();
  
  return a.exec();
}
