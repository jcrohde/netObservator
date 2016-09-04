/*
Copyright (C) 2015-2016 Jan Christian Rohde

This file is part of netObservator.

netObservator is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

netObservator is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with netObservator; if not, see http://www.gnu.org/licenses.
*/

#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <WinSock2.h>
#include "pcap.h"
#include <QMessageBox>
#include <memory>
#include <functional>
#include <set>
#include <mutex>

typedef struct ipAddress{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;

    ipAddress() {}
    ipAddress(QString str);

    bool operator==(const ipAddress &addr) const {
        return byte1 == addr.byte1 && byte2 == addr.byte2 && byte3 == addr.byte3 && byte4 == addr.byte4;
    }

    bool operator !=(const ipAddress &addr) const {
        return !(*this == addr);
    }

    bool operator<(const ipAddress &addr) const {
        return byte1 < addr.byte1
           || (byte1 == addr.byte1 && byte2 < addr.byte2)
           || (byte1 == addr.byte1 && byte2 == addr.byte2 && byte3 < addr.byte3)
           || (byte1 == addr.byte1 && byte2 == addr.byte2 && byte3 == addr.byte3 && byte4 < addr.byte4);
    }

    unsigned int length(u_char byte) const;
    void print(char *&str) const;
    QString toQString() const;

private:
    void getByteFromString(u_char &byte, QString &str);
}ipAddress;

enum column {
    TIME = 0,
    PROTOCOL = 1,
    HOSTPORT = 2,
    HOSTADDRESS = 3,
    HOSTNAME = 4,
    DIRECTION = 5,
    LOCALSRCPORT = 6,
    LOCALSRCADDRESS = 7,
    ENTIRE_PACKET = 8,
    PAYLOAD = 9
};

const unsigned int SECONDSPERDAY = 86400;

const int COLUMNNUMBER = 10;

const QString LABEL[COLUMNNUMBER] = {"Time","Protocol","HostPort","HostAddress","HostName","Direction","LocalSRCPort","LocalSRCAddress","Packet","Content"};

const QString SNIFFED = "sniffed";
const QString PACKETINFO = "packetInfo";

struct ViewComposition {
    enum class presentation {
        TABLE,
        TEXT
    };

    presentation mode;
};

struct Settings {
    bool showInfo[COLUMNNUMBER];
    unsigned int duration;

    bool save;
    QString sliceFileName;
    int sliceSize;

    Settings();
};

const QString ARBITRARY = "Arbitrary";

struct serverState{
    std::set<ipAddress> addresses;
    bool firstDocument;
    QStringList sliceNames;
    bool lastDocument;
    bool blockedBySniffThread;
    QString title;
};

class serverObserver {
public:
    virtual void update(const serverState &state) {storedState = state;}
private:
    serverState storedState;
};

struct sniffState {
    std::set<ipAddress> addresses;
    QStringList sliceNames;
    QString errorMessage;
    bool sniffing;
    bool valid;
};

class SniffObserver {
public:
    virtual void update(const sniffState &state) {storedState = state;}
private:
    sniffState storedState;
};

struct SearchCommand {
    enum Mode {
        CASEINSENSITIVE = 0,
        CASESENSITIVE = 1,
        REGEX = 2
    };

    Settings settings;
    bool invertMatch;
    Mode mode;
    QString searchString;
    QString columnName;
    bool inFiles;
    QStringList filenames;
};

typedef struct ipHeader{
    u_char versionAndInternetHeaderLength;
    u_char typeOfService;
    u_short totalLength;
    u_short identification;
    u_short flagsAndfragmentOffset;
    u_char timeToLive;
    u_char protocol;
    u_short checksum;
    ipAddress srcAddr;
    ipAddress destAddr;
    u_int optionAndPadding;
}ipHeader;

typedef struct udpHeader{
    u_short srcPort;
    u_short destPort;
    u_short length;
    u_short checksum;
}udpHeader;

struct addressItem {
    ipAddress address;
    QString hostname;
    int timeStamp;

    bool operator ==(const addressItem &item) const {
        return address == item.address;
    }

    addressItem(ipAddress addr, QString name, int time)
        : address(addr), hostname(name), timeStamp(time) {}
};

const size_t BYTESIZE = 256;

namespace std {
template<>
class hash<addressItem> {
public:
    size_t operator()(const addressItem &item) const
    {
        return ((item.address.byte1*BYTESIZE + item.address.byte2)*BYTESIZE + item.address.byte3)*BYTESIZE + item.address.byte4;
    }
};
}

enum class CommandCode {
    CLEAR,
    LOAD,
    LOADFILE,
    SAVEFILEAS,
    SAVEFILE,

    LOADSLICE,

    SHOWPACKETFILTEREDITOR,

    SHOWSTATISTICSDIALOG,
    SHOWTRAFFICDIALOG,
    SHOWBYTEDIALOG,

    SHOWSETTINGSDIALOG,

    SHOWSEARCHTABDIALOG,
    SHOWSEARCHFILEDIALOG,

    SNIFF,

    HELP,
    LICENSE,
    ABOUT
};

struct Command {
    CommandCode code;
    QStringList arguments;
};

struct traffic {
    int packetNumber;
    long int seconds;
};

Q_DECLARE_METATYPE(traffic)

extern std::mutex myCellMutex;

extern QString onLocalOrSrc(QString str);
extern QString escape(QString content);
extern QString unEscape(QString content);

extern void setErrorMessage(QString message, QMessageBox::Icon icon);

#endif // UTIL_H
