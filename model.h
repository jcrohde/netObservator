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

#ifndef MODEL_H
#define MODEL_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStandardItemModel>
#include <memory>
#include <QRegExp>
#include <QFile>
#include <QFileDialog>
#include "xmlreader.h"
#include "util.h"

class XmlFilter : private XmlReader {
public:
    bool generateFilteredString(SearchCommand cmd,QString &input, QString &output);
private:

    std::unique_ptr<QXmlStreamWriter> writer;

    SearchCommand command;

    QList<QString> subNodes;

    QRegExp regexp;

    int columnCount;
    bool found;

    void readerAction();

    void handleElementText();
    void closePacketInfo();
    void writePacketInfo();
    void reinitialize();
};

class XmlServer
{
public:
    XmlServer();
    ~XmlServer();

    bool load(QString xmlContent);
    bool search(SearchCommand command);
    void changeText(bool forward);

    QString getContent() {return output[currentOutputIter];}

    void clear();

    void registerObserver(ModelObserver *observer) {observers.push_back(observer);}
    void notifyObservers();

private:
    XmlFilter filter;

    bool changed;

    std::vector<ModelObserver*> observers;

    QList<QString> output;
    int currentOutputIter;

    bool generateFilteredString(SearchCommand cmd);
    void setAsLastDocument(QString &document);
};

#endif // MODEL_H
