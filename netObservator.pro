TEMPLATE = app


CONFIG += c++11
CONFIG -= console


QT += gui\
      widgets\
      xml


SOURCES += main.cpp \
    skratchwindow.cpp \
    menubar.cpp \
    sniffthread.cpp \
    helpdialog.cpp \
    stringfactory.cpp \
    intaction.cpp \
    methodaction.cpp \
    settingsdialog.cpp \
    xmlreader.cpp \
    searchdialog.cpp \
    util.cpp \
    devices.cpp \
    view.cpp \
    model.cpp \
    dnssingleton.cpp


INCLUDEPATH += ../WpdPack/Include \
               C:/boost/boost_1_60_0\
               C:/Users/Christian/Downloads/SDL-devel-1.2.15-mingw32/SDL-1.2.15/include


LIBS += -L WpdPack/Lib -lwpcap -lpacket\
        -lws2_32\
        -LC:/boost_1_60_0/stage/lib


HEADERS += \
    skratchwindow.h \
    menubar.h \
    sniffthread.h \
    helpdialog.h \
    stringfactory.h \
    intaction.h \
    methodaction.h \
    settingsdialog.h \
    xmlreader.h \
    searchdialog.h \
    util.h \
    devices.h \
    view.h \
    model.h \
    dnssingleton.h
