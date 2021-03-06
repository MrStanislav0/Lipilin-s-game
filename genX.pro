#-------------------------------------------------
#
# Project created by QtCreator 2017-10-17T20:43:34
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = genX
TEMPLATE = app

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
    home_window.cpp \
    qpb_modify.cpp \
    send_messege.cpp \
    fun.cpp \
    choose_button.cpp \
    root_window.cpp \
    make_img_window.cpp \
    interception.cpp \
    asimetry.cpp

HEADERS += \
        mainwindow.h \
    home_window.h \
    qpb_modify.h \
    send_messege.h \
    fun.h \
    choose_button.h \
    root_window.h \
    make_img_window.h \
    interception.h \
    asimetry.h

FORMS += \
        mainwindow.ui \
    home_window.ui \
    send_messege.ui \
    choose_button.ui \
    root_window.ui \
    make_img_window.ui \
    interception.ui \
    asimetry.ui

RESOURCES += \
    data.qrc

QMAKE_LFLAGS_RELEASE += -static -static-libgcc
CONFIG += c++11
