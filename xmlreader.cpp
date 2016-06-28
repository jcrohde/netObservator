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
#include <QBuffer>

XmlReader::XmlReader()
{

}

XmlReader::~XmlReader() {

}

bool XmlReader::read(QString xmlContent) {
    reader.reset(new QXmlStreamReader(xmlContent));

    return executeForAllPacketInfos();
}

bool XmlReader::executeForAllPacketInfos() {
    if (reader->readNextStartElement()) {
        if (reader->name() == SNIFFED){
            while (reader->readNextStartElement()) {
                if (reader->name() == PACKETINFO) {
                    while (reader->readNextStartElement()) {
                        QStringRef name = reader->name();
                        if (isPacketInfoSubnode(name)) {
                            readerAction();
                        }
                    }
                }
                else raiseError(reader->name().toString(),PACKETINFO);
            }  
        }
        else raiseError(reader->name().toString(),SNIFFED);
    }

    return getErrorState();
}

bool XmlReader::isPacketInfoSubnode(QStringRef str) {
    return (str == LABEL[TIME])
            || (str == LABEL[ADDRESS])
            || (str == LABEL[PORT])
            || (str == LABEL[HOSTNAME])
            || (str == LABEL[DIRECTION])
            || (str == LABEL[ENTIRE_PACKET])
            || (str == LABEL[PAYLOAD]);
}

void XmlReader::raiseError(QString wrong, QString correct) {
    reader->raiseError("Wrong start element " + wrong + " instead of " + correct);
}

bool XmlReader::getErrorState() {
    if (reader->error() == QXmlStreamReader::NoError)
        return true;
    else {
        setErrorMessage();
        return false;
    }
}

void XmlReader::setErrorMessage() {
    if (reader->error() == QXmlStreamReader::CustomError)
        errorMessage = reader->errorString();
    else if (reader->error() == QXmlStreamReader::NotWellFormedError)
        errorMessage = "The XML file is not well-formed";
    else
        errorMessage = "An unexpected error occured";
}
