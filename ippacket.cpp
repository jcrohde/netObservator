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

#include "ippacket.h"

ipAddress::ipAddress(QString str) {
    getByteFromString(byte1, str);
    getByteFromString(byte2, str);
    getByteFromString(byte3, str);

    byte4 = str.toInt();
}

void ipAddress::getByteFromString(u_char &byte, QString &str) {
    byte = str.left(str.indexOf(".")).toInt();
    str = str.right(str.size()-str.indexOf(".")-1);
}

unsigned int ipAddress::length(u_char byte) const {
    if (byte < 10) return 1;
    else if (byte <100) return 2;
    else return 3;
}

void ipAddress::print(char *&str) const {
    int size = length(byte1)+length(byte2)+length(byte3)+length(byte4)+4;
    str = (char*) malloc(size);
    sprintf(str,"%d.%d.%d.%d",byte1,byte2,byte3,byte4);
}

QString ipAddress::toQString() const {
    char *c;
    print(c);
    QString res(c);
    free(c);
    return res;
}
