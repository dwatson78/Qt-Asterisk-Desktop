QT += \
    core \
    gui \
    network \
    sql \
    webkit

greaterThan(QT_MAJOR_VERSION, 4) {
 QT +=  widgets webkitwidgets
}

OTHER_INC = "$$_PRO_FILE_PWD_/../3rdparty/include"
OTHER_LIBS = "$$_PRO_FILE_PWD_/../3rdparty/lib"

INCLUDEPATH += \
    /usr/include/qjson \
    /usr/include/phonon4qt5 \
    $$OTHER_INC

LIBS += \
    -lqjson -lphonon4qt5 -L$$OTHER_LIBS -lgloox -lpthread -lqt-wrap-libnotify

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
    src/admxmppwidget.cpp \
    src/admxmppbuddywidget.cpp \
    src/admxmppchatwidget.cpp \
    src/admtabwidget.cpp \
    src/admtabbar.cpp \
    src/admnotificationmanager.cpp \
    src/astchanparts.cpp \
    src/dlgphonefeatures.cpp \
    src/admxmppbuddyitem.cpp \
    src/astdialogshowevents.cpp \
    src/admintercomvideodialog.cpp \
    src/delayedamiaction.cpp

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
    src/admxmppwidget.h \
    src/admxmppbuddywidget.h \
    src/admxmppchatwidget.h \
    src/admtabwidget.h \
    src/admtabbar.h \
    src/admnotificationmanager.h \
    src/astchanparts.h \
    src/dlgphonefeatures.h \
    src/admxmppbuddyitem.h \
    src/astdialogshowevents.h \
    src/astlocalbridge.h \
    src/admintercomvideodialog.h \
    src/delayedamiaction.h

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
    src/ui/admxmppchatwidget.ui \
    src/ui/admxmppchatblockwidget.ui \
    src/ui/dlgphonefeatures.ui \
    src/ui/astdialogshowevents.ui \
    src/ui/admintercomvideodialog.ui

RESOURCES += \
    resources.qrc

TEMPLATE = app

CONFIG(release, debug|release) {
  DESTDIR = bin
}
CONFIG(debug, debug|release) {
  DESTDIR = debug
}

message("CONFIG: $${CONFIG}")

MOC_DIR = tmp/$$DESTDIR/moc
OBJECTS_DIR = tmp/$$DESTDIR/obj
UI_DIR = tmp/$$DESTDIR/ui
RCC_DIR = tmp/$$DESTDIR/rcc

#DEFINES += AST_DEBUG
