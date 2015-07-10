QT += \
    core \
    gui \
    network \
    sql \
    phonon

greaterThan(QT_MAJOR_VERSION, 4) {
 QT +=  widgets
}

OTHER_INC = "$$_PRO_FILE_PWD_/3rdparty/include"
OTHER_LIBS = "$$_PRO_FILE_PWD_/3rdparty/lib"

INCLUDEPATH += \
    "/usr/include/qjson" \
    "/usr/include/qt4/phonon" \
    $$OTHER_INC

LIBS += \
    -lqjson -lphonon -L$$OTHER_LIBS -lgloox -lpthread

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
    src/restapiastvm.cpp \
    src/glooxmsghandler.cpp \
    src/admxmppwidget.cpp \
    src/admxmppbuddywidget.cpp \
    src/admbuddytreeview.cpp \
    src/admxmppchatwidget.cpp

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
    src/restapiastvm.h \
    src/glooxmsghandler.h \
    src/admxmppwidget.h \
    src/admxmppbuddywidget.h \
    src/admbuddytreeview.h \
    src/admxmppchatwidget.h

FORMS +=     \
    src/ui/qtasteriskdesktopmain.ui \
    src/ui/qtasteriskdesktopprefs.ui \
    src/ui/astparkedcallwidget.ui \
    src/ui/admicontext.ui \
    src/ui/admcallwidget.ui \
    src/ui/admchanwidget.ui \
    src/ui/admextensionwidget.ui \
    src/ui/admvoicemailtabwidget.ui \
    src/ui/admvoicemailwidget.ui \
    src/ui/admxmppwidget.ui \
    src/ui/admxmppbuddywidget.ui \
    src/ui/admxmppchatwidget.ui

RESOURCES += \
    resources.qrc

TEMPLATE = app
DESTDIR = bin
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
UI_DIR = tmp/ui
RCC_DIR = tmp/rcc
