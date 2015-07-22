#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T15:54:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlatMesherGUI
TEMPLATE = app
CONFIG += c++11

SOURCES += src/Main.cpp\
        src/MainWindow.cpp \
    src/CollapsibleWidget.cpp \
    src/FileManager.cpp \
    src/ViewportControls.cpp

HEADERS  += include/MainWindow.h \
    include/CollapsibleWidget.h \
    include/Configuration.h \
    include/FileManager.h \
    include/ViewportControls.h

LIBS += -L"$$_PRO_FILE_PWD_/libs/" -lFlatMesher

INCLUDEPATH += include\
        ../FlatMesher/include

FORMS    += forms/MainWindow.ui

RESOURCES += \
    res/Images.qrc
