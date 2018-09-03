#-------------------------------------------------
#
# Project created by QtCreator 2018-08-01T20:32:55
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

VERSION = 1.0.0.1

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    TopWidget.cpp \
    gamemodel.cpp \
    PlayerOnLineList.cpp \
    chatwidget.cpp \
    logondialog.cpp \
    sideconfigbar.cpp \
    common.cpp \
    network.cpp \
    packetprocessor.cpp \
    popdlg.cpp \
    noticewidget.cpp \
    cheeseboardwidget.cpp \
    exitdialog.cpp

HEADERS += \
        mainwindow.h \
    TopWidget.h \
    gamemodel.h \
    PlayerOnLineList.h \
    chatwidget.h \
    logondialog.h \
    sideconfigbar.h \
    common.h \
    network.h \
    packetprocessor.h \
    popdlg.h \
    noticewidget.h \
    cheeseboardwidget.h \
    exitdialog.h

LIBS += /usr/lib/libevent.so

TARGET = FiveChess

FORMS += \
    popdlg.ui \
    exitdialog.ui

RESOURCES += \
    fivechess.qrc

RC_FILE += rcfile/FiveChess_resource.rc
