QT += \
      core \
      gui \
      network \
      sql

greaterThan(QT_MAJOR_VERSION, 4) {
 QT +=  widgets
}

TARGET = QtAsteriskDesktop

SOURCES +=   \
             src/main.cpp \
             src/qtasteriskdesktopmain.cpp \
             src/qtasteriskdesktopprefs.cpp \
             src/asteriskmanager.cpp \
             src/confbridgeuser.cpp \
             src/statusicon.cpp \
             src/astcall.cpp \
             src/astchannel.cpp \
             src/astparkedcall.cpp \
             src/admstatic.cpp \
             src/admicontext.cpp \
             src/admcallwidget.cpp

HEADERS +=   \
             src/qtasteriskdesktopmain.h \
             src/qtasteriskdesktopprefs.h \
             src/asteriskmanager.h \
             src/confbridgeuser.h \
             src/statusicon.h \
             src/astcall.h \
             src/astchannel.h \
             src/astparkedcall.h \
             src/admstatic.h \
             src/admicontext.h \
             src/admcallwidget.h

FORMS +=     \
             src/ui/qtasteriskdesktopmain.ui \
             src/ui/qtasteriskdesktopprefs.ui \
             src/ui/astparkedcallwidget.ui \
             src/ui/admicontext.ui \
             src/ui/admcallwidget.ui

RESOURCES += \
             resources.qrc

TEMPLATE = app
DESTDIR = bin
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
UI_DIR = tmp/ui
RCC_DIR = tmp/rcc
