QT += \
    core \
    gui \
    network \
    sql \
    phonon

greaterThan(QT_MAJOR_VERSION, 4) {
 QT +=  widgets
}

INCLUDEPATH += \
    "/usr/include/qjson" \
    "/usr/include/qt4/phonon"

LIBS += \
    -lqjson -lphonon

TARGET = QtAsteriskDesktop

QMAKE_CXXFLAGS += \
    -Werror

SOURCES +=   \
    src/main.cpp \
    src/qtasteriskdesktopmain.cpp \
    src/qtasteriskdesktopprefs.cpp \
    src/asteriskmanager.cpp \
    src/confbridgeuser.cpp \
    src/statusicon.cpp \
    src/astchannel.cpp \
    src/astparkedcall.cpp \
    src/admstatic.cpp \
    src/admicontext.cpp \
    src/admcallwidget.cpp \
    src/admchanwidget.cpp \
    src/astsippeer.cpp \
    src/draggabletoolbutton.cpp \
    src/admtoolboxsmoothdrag.cpp \
    src/admextensionwidget.cpp \
    src/admvoicemailtabwidget.cpp \
    src/admvoicemailwidget.cpp \
    src/restapiastvm.cpp

HEADERS +=   \
    src/qtasteriskdesktopmain.h \
    src/qtasteriskdesktopprefs.h \
    src/asteriskmanager.h \
    src/confbridgeuser.h \
    src/statusicon.h \
    src/astchannel.h \
    src/astparkedcall.h \
    src/admstatic.h \
    src/admicontext.h \
    src/admcallwidget.h \
    src/admchanwidget.h \
    src/astsippeer.h \
    src/draggabletoolbutton.h \
    src/admtoolboxsmoothdrag.h \
    src/admextensionwidget.h \
    src/admvoicemailtabwidget.h \
    src/admvoicemailwidget.h \
    src/restapiastvm.h

FORMS +=     \
    src/ui/qtasteriskdesktopmain.ui \
    src/ui/qtasteriskdesktopprefs.ui \
    src/ui/astparkedcallwidget.ui \
    src/ui/admicontext.ui \
    src/ui/admcallwidget.ui \
    src/ui/admchanwidget.ui \
    src/ui/admextensionwidget.ui \
    src/ui/admvoicemailtabwidget.ui \
    src/ui/admvoicemailwidget.ui

RESOURCES += \
    resources.qrc

TEMPLATE = app
DESTDIR = bin
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
UI_DIR = tmp/ui
RCC_DIR = tmp/rcc
