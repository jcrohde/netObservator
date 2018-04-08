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

#ifndef IPPACKET_H
#define IPPACKET_H

#include "pcap.h"
#include <set>
#include <QString>

enum class Direction {
    INBOUND = 0,
    OUTBOUND = 1,
    THIRDPARTY = 2
};

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

#endif // IPPACKET_H
