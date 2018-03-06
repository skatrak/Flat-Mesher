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

SOURCES += src/Main.cpp \
    src/MainWindow.cpp \
    src/CollapsibleWidget.cpp \
    src/FileManager.cpp \
    src/ViewportControls.cpp \
    src/MeshEditor.cpp \
    src/Configuration.cpp \
    src/GridGraphicsView.cpp \
    src/GraphicsPointItem.cpp \
    src/GraphicsLineItem.cpp \
    src/MeshAnalyzer.cpp \
    src/EditorCommands.cpp \
    src/MessageManager.cpp \
    src/CustomSpinbox.cpp

HEADERS  += include/MainWindow.h \
    include/CollapsibleWidget.h \
    include/Configuration.h \
    include/FileManager.h \
    include/ViewportControls.h \
    include/SelectionMode.h \
    include/MeshEditor.h \
    include/GridGraphicsView.h \
    include/SelectedItems.h \
    include/GraphicsPointItem.h \
    include/GraphicsLineItem.h \
    include/MeshAnalyzer.h \
    include/EditorCommands.h \
    include/MessageManager.h \
    include/CustomSpinbox.h

QMAKE_CXXFLAGS += -fopenmp

LIBS += -fopenmp -L"$$_PRO_FILE_PWD_/../library/build/" -L"$$_PRO_FILE_PWD_/libs/" -lFlatMesher

INCLUDEPATH += include \
    ../library/include

FORMS += forms/MainWindow.ui \
    forms/MeshAnalyzer.ui

RESOURCES += \
    res/Images.qrc
