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

#ifndef XMLREADER_H
#define XMLREADER_H

#include <QXmlStreamReader>
#include <memory>
#include "util.h"

class XmlReader
{
public:
    XmlReader();
    ~XmlReader();

    bool read(const QString &xmlContent);

protected:
    virtual void readerAction(QString elementText) = 0;

private:
    QXmlStreamReader reader;

    QString errorMessage;

    bool executeForAllPacketInfos();
    void readPacket();
    bool isPacketInfoSubnode(QStringRef str);
    void raiseError(QString wrong, QString correct);
    bool getErrorState();
    void writeErrorMessage();
};

class AddressExtractor : private XmlReader {
public:
    bool extract(const QString &xmlContent,std::set<ipAddress> &addr);
private:
    int counter;
    std::set<ipAddress> addressSet;
    void readerAction(QString elementText);
};

#endif // XMLREADER_H
