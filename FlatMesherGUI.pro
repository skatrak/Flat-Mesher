#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T15:54:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlatMesherGUI
TEMPLATE = app

SOURCES += src/Main.cpp\
        src/MainWindow.cpp \
    src/CollapsibleWidget.cpp

HEADERS  += include/MainWindow.h \
    include/CollapsibleWidget.h \
    include/Configuration.h

LIBS += -L"$$_PRO_FILE_PWD_/libs/" -lFlatMesher

INCLUDEPATH += include\
        ../FlatMesher/include

FORMS    += forms/MainWindow.ui

RESOURCES += \
    res/Images.qrc
