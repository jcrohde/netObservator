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

#ifndef XMLSERVER_H
#define XMLSERVER_H

#include "xmlreader.h"

class XmlFilter : private XmlReader {
public:
    bool generateFilteredXmlDocument(const SearchCommand &cmd, QString &input, QString &output);
private:

    std::unique_ptr<QXmlStreamWriter> writer;

    SearchCommand command;

    QList<QString> subNodes;

    QRegExp regexp;

    int columnCount;
    bool found;

    void readerAction(QString elementText);

    void handleElementText(QString &elementText);
    void closePacketInfo();
    void writePacketInfo();
    void reinitialize();
};

class XmlServer
{
public:
    XmlServer(XmlFilter *xmlFilter);
    ~XmlServer();

    void load(QString xmlContent, std::set<ipAddress> addr);
    bool search(SearchCommand command);
    void changeText(bool forward);
    void setTitle(QString t) {title = t; changed = true;}

    const QString &getContent() {return output[currentOutputIter];}
    void getState(serverState &state);

    bool change() {return changed;}

    void clear();

    void setSniffed(const QStringList &slices);

    const QStringList &getSliceNames() {return sliceNames;}

private:
    QStringList sliceNames;

    XmlFilter *filter;

    bool changed;

    std::set<ipAddress> addresses;

    QList<QString> output;
    int currentOutputIter;

    QString title;

    void setAsLastDocument(QString &document);
};
#endif // XMLSERVER_H
