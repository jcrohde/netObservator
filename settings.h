/*
Copyright (C) 2015-2018 Jan Christian Rohde

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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QStringList>

enum column {
    TIME = 0,
    VERSIONANDINTERNETHEADERLENGHT = 1,
    TYPEOFSERVICE = 2,
    TOTALLENGTH = 3,
    IDENTIFICATION = 4,
    FLAGSANDFRAGMENTOFFSET = 5,
    TIMETOLIFE = 6,
    PROTOCOL = 7,
    HOSTPORT = 8,
    HOSTADDRESS = 9,
    HOSTNAME = 10,
    DIRECTION = 11,
    LOCALSRCPORT = 12,
    LOCALSRCADDRESS = 13,
    OPTIONANDPADDING = 14,
    UDPLENGTH = 15,
    ENTIRE_PACKET = 16,
    PAYLOAD = 17,
    IPHEADERCHECKSUM = 18,
    UDPHEADERCHECKSUM = 19,
};


const int COLUMNNUMBER = 20;

const QString LABEL[COLUMNNUMBER] = {"Time",
                                     "VersionInternetHeaderLength",
                                     "TOS",
                                     "TotalLength",
                                     "Identification",
                                     "FlagsAndfragmentOffset",
                                     "TTL",
                                     "Protocol",
                                     "HostPort",
                                     "HostAddress",
                                     "HostName",
                                     "Direction",
                                     "LocalSRCPort",
                                     "LocalSRCAddress",
                                     "OptionAndPadding",
                                     "UdpLength",
                                     "Packet",
                                     "Content",
                                     "IpHeaderCheckSum",
                                     "UdpHeaderCheckSum"
                                    };

struct viewSettings {
    bool showInfo[COLUMNNUMBER];

    int shownColumns;

    viewSettings();
};

struct sniffSettings {
    enum class Mode {
        SAVEANDSHOW = 0,
        JUSTSAVE = 1,
        JUSTSHOW = 2
    };

    unsigned int duration;

    Mode mode;
    QString storeFolderName;
    QString sliceFileName;
    int sliceSize;

    sniffSettings();
};

struct SearchCommand {
    enum Mode {
        CASEINSENSITIVE = 0,
        CASESENSITIVE = 1,
        REGEX = 2
    };

    viewSettings settings;
    bool invertMatch;
    Mode mode;
    QString searchString;
    int column;
    bool inFiles;
    QStringList filenames;
};

extern QString onLocalOrSrc(QString str);

#endif // SETTINGS_H
