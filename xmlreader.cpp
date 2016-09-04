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

#include "xmlreader.h"
#include "dnssingleton.h"
#include <QBuffer>

XmlReader::XmlReader() {

}

XmlReader::~XmlReader() {

}

bool XmlReader::read(const QString &xmlContent) {
    reader.clear();
    reader.addData(xmlContent);

    return executeForAllPacketInfos();
}

bool XmlReader::executeForAllPacketInfos() {
    if (reader.readNextStartElement()) {
        if (reader.name() == SNIFFED){
            while (reader.readNextStartElement()) {
                if (reader.name() == PACKETINFO) {
                    readPacket();
                }
                else raiseError(reader.name().toString(),PACKETINFO);
            }  
        }
        else raiseError(reader.name().toString(),SNIFFED);
    }

    return getErrorState();
}

void XmlReader::readPacket() {
    int propertyCount = 0;
    while (reader.readNextStartElement()) {
        QStringRef name = reader.name();
        if (isPacketInfoSubnode(name)) {
            readerAction(reader.readElementText());
        }
        else
            reader.raiseError("Wrong name of a packet property.");
        propertyCount++;
    }
    if (propertyCount != COLUMNNUMBER)
        reader.raiseError("Wrong number of properties in a packet.");
}

bool XmlReader::isPacketInfoSubnode(QStringRef str) {
    return (str == LABEL[TIME])
            || (str == LABEL[PROTOCOL])
            || (str == LABEL[HOSTADDRESS])
            || (str == LABEL[HOSTPORT])
            || (str == LABEL[HOSTNAME])
            || (str == LABEL[DIRECTION])
            || (str == LABEL[LOCALSRCPORT])
            || (str == LABEL[LOCALSRCADDRESS])
            || (str == LABEL[ENTIRE_PACKET])
            || (str == LABEL[PAYLOAD]);
}

void XmlReader::raiseError(QString wrong, QString correct) {
    reader.raiseError("Wrong start element \"" + wrong + "\" instead of \"" + correct + "\".");
}

bool XmlReader::getErrorState() {
    if (reader.error() == QXmlStreamReader::NoError)
        return true;
    else {
        writeErrorMessage();
        setErrorMessage("Can not parse the XML file: " + errorMessage, QMessageBox::Critical);
        return false;
    }
}

void XmlReader::writeErrorMessage() {
    if (reader.error() == QXmlStreamReader::CustomError)
        errorMessage = reader.errorString();
    else if (reader.error() == QXmlStreamReader::NotWellFormedError)
        errorMessage = "The XML file is not well-formed";
    else
        errorMessage = "An unexpected error occured";
}

bool AddressExtractor::extract(const QString &xmlContent, std::set<ipAddress> &addr) {
    addressSet.clear();
    counter = 0;

    bool valid = read(xmlContent);

    if (valid) addr = addressSet;

    return valid;
}

void AddressExtractor::readerAction(QString elementText) {
    if ((counter % COLUMNNUMBER) == HOSTADDRESS) {
        ipAddress addr(elementText);
        addressSet.insert(addr);
    }
    counter++;
}
