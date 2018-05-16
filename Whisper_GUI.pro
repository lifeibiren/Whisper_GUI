#-------------------------------------------------
#
# Project created by QtCreator 2018-05-03T20:50:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Whisper_GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS BOOST_ENDIAN_DEPRECATED_NAMES

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    settingdialog.cpp \
    peerlist.cpp \
    trustedpeerstablemodel.cpp \
    chatcontext.cpp

HEADERS += \
        mainwindow.h \
    settingdialog.h \
    peerlist.h \
    trustedpeerstablemodel.h \
    yamlcpp-qt.h \
    chatcontext.h

FORMS += \
        mainwindow.ui \
    settingdialog.ui \
    peerlist.ui

#QMAKE_CXXFLAGS_DEBUG += -fsanitize=address,leak,undefined -g3
#QMAKE_CFLAGS_DEBUG += -fsanitize=address,leak,undefined -g3
#QMAKE_LFLAGS += -fsanitize=address,leak,undefined -g3
INCLUDEPATH += ../Whisper/client/sml/
LIBS += -L"../build-Whisper-Desktop_Clang-Debug/client/sml/" -lsml -lboost_system -lboost_thread -lyaml-cpp -lcryptopp -ldl
