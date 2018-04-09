TEMPLATE = app


CONFIG += c++11
CONFIG -= console


QT += gui\
      widgets\
      xml\
      charts\
      core\


SOURCES += main.cpp \
    skratchwindow.cpp \
    menubar.cpp \
    sniffthread.cpp \
    helpdialog.cpp \
    stringfactory.cpp \
    intaction.cpp \
    settingsdialog.cpp \
    searchdialog.cpp \
    util.cpp \
    devices.cpp \
    view.cpp \
    model.cpp \
    dnssingleton.cpp \
    packetinfopresenter.cpp \
    dnsthread.cpp \
    packetfiltereditor.cpp \
    displaytab.cpp \
    controller.cpp \
    xmlserver.cpp \
    searchstrategy.cpp \
    viewdialog.cpp \
    trafficdialog.cpp \
    toolbar.cpp \
    packetparser.cpp \
    settings.cpp \
    ippacket.cpp \
    observers.cpp \
    command.cpp \
    viewsettingsdialog.cpp \
    parsescheme.cpp \
    hostchart.cpp


INCLUDEPATH += C:/Users/jan-c/Downloads/WpdPack_4_1_2/WpdPack/Include \
               C:/boost/boost_1_60_0\
               C:/Users/Christian/Downloads/SDL-devel-1.2.15-mingw32/SDL-1.2.15/include


LIBS += -L C:/Users/jan-c/Downloads/WpdPack_4_1_2/WpdPack/Lib -lwpcap -lpacket\
        -lws2_32\
        -LC:/boost_1_60_0/stage/lib


HEADERS += \
    skratchwindow.h \
    menubar.h \
    sniffthread.h \
    helpdialog.h \
    stringfactory.h \
    intaction.h \
    settingsdialog.h \
    searchdialog.h \
    util.h \
    devices.h \
    view.h \
    model.h \
    dnssingleton.h \
    packetinfopresenter.h \
    dnsthread.h \
    packetfiltereditor.h \
    displaytab.h \
    controller.h \
    modelview.h \
    xmlserver.h \
    searchstrategy.h \
    viewdialog.h \
    trafficdialog.h \
    toolbar.h \
    packetparser.h \
    settings.h \
    ippacket.h \
    observers.h \
    command.h \
    viewsettingsdialog.h \
    viewsettingsdialog.h \
    parsescheme.h \
    hostchart.h
